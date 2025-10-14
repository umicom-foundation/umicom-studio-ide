/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 *
 * PURPOSE:
 *   Minimal GLib/GIO-based process runner that streams stdout/stderr lines
 *   to a decoupled UmiOutputSink. Used by build systems and search tools.
 *
 * SECURITY:
 *   - No shell invocation; argv is passed as an array (no injection).
 *   - Environment/cwd are applied explicitly via GSubprocessLauncher.
 *
 * KEY POINTS:
 *   - No UI includes here; depends only on umi_output_sink.h (+ diagnostics).
 *   - We read stdout and (optionally) stderr concurrently on two GLib threads
 *     to avoid pipe-buffer deadlocks.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gio/gio.h>
#include <string.h>

#include "build_runner.h"         /* public runner API (opaque type)       */
#include "umi_diagnostics.h"      /* UmiDiag + umi_diag_free               */

/*-----------------------------------------------------------------------------
 * Private structure — not exposed in the header (keeps ABI/API clean).
 *---------------------------------------------------------------------------*/
struct UmiBuildRunner {
  UmiOutputSink *sink;            /* not owned; caller manages lifetime    */
};

/* Constructor / Destructor / Sink setter */
UmiBuildRunner *umi_build_runner_new(void) { return g_new0(UmiBuildRunner, 1); }
void umi_build_runner_free(UmiBuildRunner *br) { g_free(br); }
void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink *sink)
{
  if (!br) return;
  br->sink = sink;
}

/* Emit one line to the sink with a chosen severity (note for stdout, warning
 * for stderr by default). The sink stays UI-agnostic; we transport diagnostics.
 */
static inline void emit_line(UmiOutputSink *sink,
                             UmiDiagSeverity sev,
                             const char *line)
{
  if (!sink || !line) return;

  UmiDiag *d = g_new0(UmiDiag, 1);
  d->severity = sev;
  d->file     = g_strdup("");      /* unknown file for plain process lines */
  d->line     = 0;
  d->column   = 0;
  d->message  = g_strdup(line);
  umi_output_sink_emit(sink, d);
  umi_diag_free(d);
}

/* Synchronous line reader for a GInputStream; called on dedicated threads. */
typedef struct ReaderCtx {
  GInputStream  *stream;          /* borrowed; we hold a ref               */
  gboolean       is_err;          /* stderr? → different default severity  */
  UmiOutputSink *sink;            /* borrowed; not owned                   */
} ReaderCtx;

static gpointer reader_thread(gpointer data)
{
  ReaderCtx *ctx = (ReaderCtx *)data;
  if (!ctx || !ctx->stream || !ctx->sink) { g_free(ctx); return NULL; }

  GDataInputStream *din = g_data_input_stream_new(ctx->stream);
  for (;;) {
    gsize len = 0;
    GError *e = NULL;
    gchar *line = g_data_input_stream_read_line(din, &len, NULL, &e);
    if (e)       { g_clear_error(&e); g_free(line); break; }
    if (!line)   { break; }
    emit_line(ctx->sink, ctx->is_err ? UMI_DIAG_WARNING : UMI_DIAG_NOTE, line);
    g_free(line);
  }
  g_object_unref(din);
  g_object_unref(ctx->stream);
  g_free(ctx);
  return NULL;
}

/* Build a NULL-terminated vector: { exe, argv[0], argv[1], ..., NULL } */
static gchar **build_vector_with_exe(const char *exe, const char * const argv[])
{
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  g_ptr_array_add(a, g_strdup(exe));
  if (argv) { for (guint i = 0; argv[i]; ++i) g_ptr_array_add(a, g_strdup(argv[i])); }
  g_ptr_array_add(a, NULL);
  return (gchar**)g_ptr_array_free(a, FALSE); /* transfer backing array */
}

/* Public run(): spawns the child, streams output, and returns TRUE on exit 0. */
gboolean umi_build_runner_run(UmiBuildRunner        *br,
                              const char            *cwd,
                              const char            *exe,
                              const char * const     argv[],
                              const char * const     envp[],
                              gboolean               merge_stderr)
{
  if (!br || !exe) return FALSE;

  /* Configure launcher with required pipes. */
  GSubprocessLauncher *launcher = g_subprocess_launcher_new(
      G_SUBPROCESS_FLAGS_STDOUT_PIPE |
      (merge_stderr ? G_SUBPROCESS_FLAGS_STDERR_MERGE : G_SUBPROCESS_FLAGS_STDERR_PIPE));

  if (cwd && *cwd)
    g_subprocess_launcher_set_cwd(launcher, cwd);

  /* Apply environment (duplicate into launcher, keeping our inputs const). */
  if (envp) {
    for (guint i = 0; envp[i]; ++i) {
      const char *eq = strchr(envp[i], '=');
      if (!eq) continue;
      gchar *k = g_strndup(envp[i], (gsize)(eq - envp[i]));
      g_subprocess_launcher_setenv(launcher, k, eq + 1, TRUE /*overwrite*/);
      g_free(k);
    }
  }

  /* Spawn the process. */
  gchar **argvv = build_vector_with_exe(exe, argv);
  GError *err = NULL;
  GSubprocess *sp = g_subprocess_launcher_spawnv(launcher,
                                                 (const gchar * const *)argvv,
                                                 &err);
  g_object_unref(launcher);
  g_strfreev(argvv);

  if (!sp) {
    if (br->sink) emit_line(br->sink, UMI_DIAG_ERROR,
                            err && err->message ? err->message : "spawn failed");
    g_clear_error(&err);
    return FALSE;
  }

  /* Start readers (stdout + optional stderr). */
  GThread *th_out = NULL, *th_err = NULL;

  {
    ReaderCtx *ctx = g_new0(ReaderCtx, 1);
    ctx->stream = G_INPUT_STREAM(g_object_ref(g_subprocess_get_stdout_pipe(sp)));
    ctx->is_err = FALSE;
    ctx->sink   = br->sink;
    th_out = g_thread_new("umi-br-out", reader_thread, ctx);
  }

  if (!merge_stderr) {
    ReaderCtx *ctx = g_new0(ReaderCtx, 1);
    ctx->stream = G_INPUT_STREAM(g_object_ref(g_subprocess_get_stderr_pipe(sp)));
    ctx->is_err = TRUE;
    ctx->sink   = br->sink;
    th_err = g_thread_new("umi-br-err", reader_thread, ctx);
  }

  /* Wait for child to finish, then join readers. */
  gboolean ok = g_subprocess_wait_check(sp, NULL, &err);  /* exit==0 → ok */
  if (err) {                                              /* collect any wait error */
    if (br->sink) emit_line(br->sink, UMI_DIAG_ERROR, err->message);
    g_clear_error(&err);
  }

  g_thread_join(th_out);
  if (th_err) g_thread_join(th_err);

  g_object_unref(sp);
  return ok;
}

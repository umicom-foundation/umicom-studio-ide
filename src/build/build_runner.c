/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
*
 * PROJECT:
 *   Umicom Studio IDE — build subsystem
 *
 * PURPOSE:
 *   Launches external processes using GIO (GSubprocess), streams stdout/stderr
 *   asynchronously, and notifies the caller on exit.
  *   Spawn external tools (build/run) via GLib/GIO, capture stdout/stderr
 *   asynchronously, and notify the caller on process exit. This module is
 *   strictly confined to the "build" subsystem; other modules call us via
 *   our own public header (build_runner.h) only — no cross-module includes.
 *
 * DESIGN NOTES:
 *   - Pure C (C11) + GLib/GIO (no GTK here).
 *   - Opaque runner type: callers cannot touch internals ⇒ loose coupling.
 *   - Asynchronous, line-oriented IO using GDataInputStream.
 *   - Optional sink callback for output lines (stdout/stderr).
 *   - Optional exit callback with normalized exit code.
 *
 * SAFETY:
 *   - No function-pointer UB; all callbacks have exact, declared types.
 *   - No buffer overflows: GLib allocates lines; we free them with g_free().
 *   - No dangling pointers: every GObject is g_clear_object()’d.
 *   - No environment aliasing: we duplicate envp into a mutable GStrv and
 *     *transfer ownership* to the launcher (we do NOT free it afterwards).
 * REQUIREMENTS:
 *   - GLib (gio-2.0, glib-2.0) available at build/link time.
 * - Optional UmiOutputSink receives each text line + a stderr flag.
 *   - Env is inherited by default; custom envp supported (internally duplicated).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"      /* Public contract for this module              */
#include <string.h>            /* strchr, etc. (not strictly required)        */

/*------------------------------- Internals ----------------------------------*/
/* Forward declarations for file-local helpers.                                */
static void read_next_stdout(struct _UmiBuildRunner *br);
static void read_next_stderr(struct _UmiBuildRunner *br);
static void append_line      (struct _UmiBuildRunner *br, const char *s, gboolean is_err);
static void on_read_stdout   (GObject *src, GAsyncResult *res, gpointer user);
static void on_read_stderr   (GObject *src, GAsyncResult *res, gpointer user);
static void on_proc_exited   (GObject *src, GAsyncResult *res, gpointer user);

/* The private struct (opaque to callers; defined only here).                  */
struct _UmiBuildRunner {
  GSubprocess       *proc;     /* running child process                        */
  GDataInputStream  *out_r;    /* async line reader for stdout                 */
  GDataInputStream  *err_r;    /* async line reader for stderr                 */
  GCancellable      *cancel;   /* cancels pending async reads                  */
  UmiBuildExitCb     on_exit;  /* optional exit callback                       */
  gpointer           user;     /* opaque user pointer for callbacks            */
  UmiOutputSink      sink;     /* optional line sink (stdout/stderr)           */
  gpointer           sink_user;/* sink user data                               */
};

/*------------------------------- API ----------------------------------------*/
UmiBuildRunner *umi_build_runner_new(void)
{
  UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);   /* zero-init fields        */
  br->cancel = g_cancellable_new();                 /* create cancellable      */
  return br;                                        /* hand to caller          */
}

void umi_build_runner_free(UmiBuildRunner *br)
{
  if (!br) return;                                  /* guard                   */
  g_clear_object(&br->out_r);                       /* drop stdout reader      */
  g_clear_object(&br->err_r);                       /* drop stderr reader      */
  g_clear_object(&br->proc);                        /* drop child handle       */
  g_clear_object(&br->cancel);                      /* drop cancellable        */
  g_free(br);                                       /* free struct             */
}

void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user)
{
  if (!br) return;                                  /* guard                   */
  br->sink      = sink;                             /* save sink function      */
  br->sink_user = sink_user;                        /* save sink user data     */
}

/* Launch the child process and start async reads.                              */
gboolean umi_build_runner_run(UmiBuildRunner *br,
                              char * const *argv,
                              char * const *envp,
                              const char   *cwd,
                              UmiBuildExitCb on_exit,
                              gpointer       user,
                              GError       **err)
{
  if (!br || !argv || !argv[0]) {                   /* validate inputs         */
    g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                "umi_build_runner_run: invalid arguments");
    return FALSE;
  }

  /* Save exit callback & user context.                                        */
  br->on_exit = on_exit;
  br->user    = user;

  /* Configure launcher: capture stdout/stderr as pipes for line streaming.    */
  GSubprocessLauncher *launch =
      g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                G_SUBPROCESS_FLAGS_STDERR_PIPE);

  if (cwd && *cwd) {                                /* set working directory   */
    g_subprocess_launcher_set_cwd(launch, cwd);
  }

  /* GLib requires a mutable GStrv for the environment; make a duplicate.      */
  if (envp) {
    gchar **dup = g_strdupv((gchar * const *)envp); /* writable copy           */
    g_subprocess_launcher_set_environ(launch, dup); /* set child environ       */
    g_strfreev(dup);                                /* free our copy           */
  } else {
    gchar **env = g_get_environ();                  /* snapshot current env    */
    g_subprocess_launcher_set_environ(launch, env); /* set child environ       */
    g_strfreev(env);                                /* free snapshot           */
  }

  /* Spawn the child. Note: API takes const gchar * const * for argv.          */
  GError *lerr = NULL;
  br->proc = g_subprocess_launcher_spawnv(
      launch,
      (const gchar * const *)argv,                  /* cast allowed: read-only */
      &lerr
  );
  g_object_unref(launch);                           /* release launcher        */

  if (!br->proc) {                                  /* spawn failed            */
    if (lerr) g_propagate_error(err, lerr);
    return FALSE;
  }

  /* Get pipes and wrap them in line-reading streams.                           */
  GInputStream *out_s = g_subprocess_get_stdout_pipe(br->proc);
  GInputStream *err_s = g_subprocess_get_stderr_pipe(br->proc);
  br->out_r = g_data_input_stream_new(out_s);
  br->err_r = g_data_input_stream_new(err_s);

  /* Begin reading lines asynchronously from both streams.                      */
  read_next_stdout(br);
  read_next_stderr(br);

  /* Notify when the process exits (non-blocking).                               */
  g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);

  return TRUE;                                      /* launch OK               */
}

/* Best-effort stop: cancel I/O and ask the child to exit.                      */
void umi_build_runner_stop(UmiBuildRunner *br)
{
  if (!br) return;                                  /* guard                   */
  g_cancellable_cancel(br->cancel);                 /* cancel pending reads    */
  if (br->proc) g_subprocess_force_exit(br->proc);  /* request child exit      */
  append_line(br, "[runner] stop requested", TRUE); /* route diagnostic line   */
}

/*------------------------------ Helpers -------------------------------------*/
static void append_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
  if (br && br->sink) br->sink(br->sink_user, s ? s : "", is_err);
}

static void read_next_stdout(UmiBuildRunner *br)
{
  if (!br || !br->out_r) return;
  g_data_input_stream_read_line_async(
      br->out_r, G_PRIORITY_DEFAULT, br->cancel, on_read_stdout, br);
}

static void read_next_stderr(UmiBuildRunner *br)
{
  if (!br || !br->err_r) return;
  g_data_input_stream_read_line_async(
      br->err_r, G_PRIORITY_DEFAULT, br->cancel, on_read_stderr, br);
}

static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;
  gsize len = 0;
  GError *gerr = NULL;
  gchar *line = g_data_input_stream_read_line_finish(
      G_DATA_INPUT_STREAM(src), res, &len, &gerr);

  if (gerr) { append_line(br, gerr->message, TRUE); g_error_free(gerr); return; }
  if (line)  { append_line(br, line, FALSE); g_free(line); read_next_stdout(br); }
  /* NULL line and no error => EOF; stop. */
}

static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;
  gsize len = 0;
  GError *gerr = NULL;
  gchar *line = g_data_input_stream_read_line_finish(
      G_DATA_INPUT_STREAM(src), res, &len, &gerr);

  if (gerr) { append_line(br, gerr->message, TRUE); g_error_free(gerr); return; }
  if (line)  { append_line(br, line, TRUE);  g_free(line); read_next_stderr(br); }
}

static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;
  GError *gerr = NULL;
  if (!g_subprocess_wait_finish(G_SUBPROCESS(src), res, &gerr)) {
    /* Propagate wait error to sink if any.                                     */
    if (gerr) { append_line(br, gerr->message, TRUE); g_error_free(gerr); }
  }

  /* Report exit status to callback (if present).                               */
  if (br->on_exit) {
    int code = g_subprocess_get_exit_status(G_SUBPROCESS(src));
    br->on_exit(br->user, code);
  }
}


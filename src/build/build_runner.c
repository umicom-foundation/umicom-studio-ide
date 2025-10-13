/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 *
 * PROJECT:
 *   Umicom Studio IDE — build subsystem
 *
 * PURPOSE:
 *   Spawn external tools (build/run) using GLib/GIO (GSubprocess), capture
 *   stdout/stderr asynchronously on the GLib main context, and notify callers
 *   when the process exits. This runner is used by higher-level build UI to
 *   stream process output lines and to observe the normalized exit status.
 *
 * PUBLIC API (see include/build_runner.h):
 *   UmiBuildRunner *umi_build_runner_new(void);
 *   void            umi_build_runner_free(UmiBuildRunner *br);
 *   void            umi_build_runner_set_sink(UmiBuildRunner *br,
 *                                             UmiOutputSink sink,
 *                                             gpointer      sink_user);
 *   gboolean        umi_build_runner_run(UmiBuildRunner   *br,
 *                                        char * const     *argv,
 *                                        char * const     *envp,
 *                                        const char       *cwd,
 *                                        UmiBuildExitCb    on_exit,
 *                                        gpointer          user,
 *                                        GError          **err);
 *   void            umi_build_runner_stop(UmiBuildRunner *br);
 *
 * DESIGN / NOTES:
 *   - Opaque runner type: callers cannot access internals (defined here only).
 *   - Asynchronous, line-oriented IO via GDataInputStream::read_line_async().
 *   - Output is pushed to an optional UmiOutputSink as (user, line, is_err).
 *   - Exit notification uses UmiBuildExitCb(user, exit_code).
 *   - We never block the main loop: spawn + IO + wait are async.
 *
 * SAFETY:
 *   - Every GObject is owned and released (g_clear_object) on free().
 *   - Lines are allocated by GLib; we free them with g_free().
 *   - Environment duplication: we avoid casting away const by duplicating
 *     the environment vector with a tiny helper that accepts
 *     (const char * const *), eliminating -Wdiscarded-qualifiers.
 *
 * COMPAT:
 *   - C11 + GLib/GIO. No GTK dependencies in this file.
 *   - The function signatures match build_runner.h exactly.
 *
 * THREADING:
 *   - Intended for use from the main thread / GLib main context. IO callbacks
 *     will dispatch there. Stop() cancels outstanding IO via GCancellable.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"          /* Public contract for this module          */
#include <gio/gio.h>               /* GSubprocess, GDataInputStream, etc.     >
#include <string.h>                /* Optional utilities (not strictly used)  */

/*=============================================================================
 * File-local types and helpers
 *===========================================================================*/

/* Forward declarations for file-local helper routines.                        */
static void         read_next_stdout(struct _UmiBuildRunner *br);
static void         read_next_stderr(struct _UmiBuildRunner *br);
static void         append_line      (struct _UmiBuildRunner *br,
                                      const char *s,
                                      gboolean    is_err);
static void         on_read_stdout   (GObject      *src,
                                      GAsyncResult *res,
                                      gpointer      user);
static void         on_read_stderr   (GObject      *src,
                                      GAsyncResult *res,
                                      gpointer      user);
static void         on_proc_exited   (GObject      *src,
                                      GAsyncResult *res,
                                      gpointer      user);

/*------------------------------------------------------------------------------
 * Small utility: duplicate a NULL-terminated string vector safely.
 *
 * Why not g_strdupv? g_strdupv takes (gchar **), not (const gchar * const **),
 * so passing a const vector triggers -Wdiscarded-qualifiers. This helper keeps
 * the interface const-correct and returns an owning duplicate you can free with
 * g_strfreev(). It is intentionally tiny and boring.
 *----------------------------------------------------------------------------*/
static char **dup_strv(const char * const *in)
{
  if (!in) return NULL;                /* no input => no output                */
  /* Count entries. */
  size_t n = 0;
  for (const char * const *p = in; *p; ++p) n++;

  /* Allocate (n + 1) for trailing NULL. */
  char **out = g_new0(char *, n + 1);
  for (size_t i = 0; i < n; ++i) {
    out[i] = g_strdup(in[i]);          /* duplicate each C string              */
  }
  /* out[n] already NULL due to g_new0. */
  return out;                          /* caller frees with g_strfreev()       */
}

/*------------------------------------------------------------------------------
 * The private runner struct (opaque outside this file).
 * Callers only reference UmiBuildRunner * and configure via setters.
 *----------------------------------------------------------------------------*/
struct _UmiBuildRunner {
  /* Child process + IO plumbing -------------------------------------------- */
  GSubprocess       *proc;     /* Handle to the running child process         */
  GDataInputStream  *out_r;    /* Line-reader wrapping child's STDOUT         */
  GDataInputStream  *err_r;    /* Line-reader wrapping child's STDERR         */
  GCancellable      *cancel;   /* Cancels pending async operations            */

  /* Callbacks supplied by the caller --------------------------------------- */
  UmiBuildExitCb     on_exit;  /* Notified when the process exits             */
  gpointer           user;     /* Opaque user pointer passed to callbacks     */

  /* Optional sink for each output line (stdout/stderr) --------------------- */
  UmiOutputSink      sink;     /* (user, line, is_err)                        */
  gpointer           sink_user;/* Opaque sink user pointer                    */
};

/*=============================================================================
 * Public API
 *===========================================================================*/

/*------------------------------------------------------------------------------
 * Construct a new runner instance.
 * We allocate and zero-init the struct; GCancellable created for later use.
 *----------------------------------------------------------------------------*/
UmiBuildRunner *
umi_build_runner_new(void)
{
  UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);   /* zero-init all fields    */
  br->cancel = g_cancellable_new();                 /* dedicated cancellable   */
  return br;                                        /* caller takes ownership  */
}

/*------------------------------------------------------------------------------
 * Free the runner and all its resources.
 * Safe with NULL.
 *----------------------------------------------------------------------------*/
void
umi_build_runner_free(UmiBuildRunner *br)
{
  if (!br) return;                                  /* guard                   */

  /* Cancel outstanding IO (no-op if not pending). */
  if (br->cancel) g_cancellable_cancel(br->cancel);

  /* Release GObjects in reverse allocation order. */
  g_clear_object(&br->out_r);
  g_clear_object(&br->err_r);
  g_clear_object(&br->proc);
  g_clear_object(&br->cancel);

  /* Finally free the struct itself. */
  g_free(br);
}

/*------------------------------------------------------------------------------
 * Configure an optional sink to receive each output line.
 * The sink is called from the GLib main context thread as lines arrive.
 *----------------------------------------------------------------------------*/
void
umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user)
{
  if (!br) return;                                  /* guard                   */
  br->sink      = sink;
  br->sink_user = sink_user;
}

/*------------------------------------------------------------------------------
 * Start the process, wire up async IO, and set up exit notification.
 *
 * argv  : NULL-terminated vector; argv[0] must be the program path.
 * envp  : NULL to inherit the current environment; otherwise, a NULL-terminated
 *         vector of "KEY=VALUE" entries (we duplicate it internally).
 * cwd   : Optional working directory; NULL to inherit.
 * on_exit/user: optional completion callback and its user pointer.
 *
 * Returns TRUE on successful spawn; FALSE and sets @err otherwise.
 *----------------------------------------------------------------------------*/
gboolean
umi_build_runner_run(UmiBuildRunner  *br,
                     char * const    *argv,
                     char * const    *envp,
                     const char      *cwd,
                     UmiBuildExitCb   on_exit,
                     gpointer         user,
                     GError         **err)
{
  /* Validate essential inputs early to fail fast. */
  if (!br || !argv || !argv[0]) {
    g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                "umi_build_runner_run: invalid arguments (runner/argv)");
    return FALSE;
  }

  /* Persist exit callback context for later. */
  br->on_exit = on_exit;
  br->user    = user;

  /* Create a launcher that captures stdout/stderr as pipes. */
  GSubprocessLauncher *launch =
      g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                G_SUBPROCESS_FLAGS_STDERR_PIPE);

  /* Apply working directory if provided. */
  if (cwd && *cwd) {
    g_subprocess_launcher_set_cwd(launch, cwd);
  }

  /* Environment handling:
   *  - If envp == NULL, we snapshot the current process environment (inherit).
   *  - If envp != NULL, we duplicate it WITHOUT casting away const.
   *  - We then set the duplicated vector on the launcher.
   *  - GLib copies the content for the launcher; we free our duplicate.
   */
  if (envp) {
    char **dup = dup_strv((const char * const *)envp);   /* const-correct dup  */
    g_subprocess_launcher_set_environ(launch, dup);      /* set child env      */
    g_strfreev(dup);                                     /* free our duplicate */
  } else {
    char **inherit = g_get_environ();                    /* snapshot current   */
    g_subprocess_launcher_set_environ(launch, inherit);  /* set child env      */
    g_strfreev(inherit);                                 /* free snapshot      */
  }

  /* Spawn the child process. Note: spawnv expects (const gchar * const *). */
  GError *spawn_err = NULL;
  br->proc = g_subprocess_launcher_spawnv(
      launch,
      (const gchar * const *)argv,                       /* safe cast (read-only) */
      &spawn_err
  );

  /* Launcher no longer needed; release promptly. */
  g_object_unref(launch);

  /* Handle spawn failure (nothing else to clean up yet). */
  if (!br->proc) {
    if (spawn_err) g_propagate_error(err, spawn_err);
    return FALSE;
  }

  /* Wrap the child's stdout/stderr pipes with line-oriented readers. */
  GInputStream *out_s = g_subprocess_get_stdout_pipe(br->proc);
  GInputStream *err_s = g_subprocess_get_stderr_pipe(br->proc);
  br->out_r = g_data_input_stream_new(out_s);
  br->err_r = g_data_input_stream_new(err_s);

  /* Kick off the first async read on each stream. Each completion schedules
   * the next read until EOF or cancellation is observed. */
  read_next_stdout(br);
  read_next_stderr(br);

  /* Arrange for exit notification without blocking. */
  g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);

  return TRUE;                                        /* launched successfully */
}

/*------------------------------------------------------------------------------
 * Best-effort stop:
 *   - Cancel any pending async reads.
 *   - Politely request the child to exit (force_exit).
 *   - Emit a diagnostic line through the sink (if present).
 * Note: If the child ignores SIGTERM semantics on Windows, force_exit will
 *       terminate it unconditionally.
 *----------------------------------------------------------------------------*/
void
umi_build_runner_stop(UmiBuildRunner *br)
{
  if (!br) return;                                  /* guard                   */

  /* Cancel any pending IO first; this will unwind callbacks quickly. */
  if (br->cancel) g_cancellable_cancel(br->cancel);

  /* Request the subprocess to exit if still running. */
  if (br->proc) g_subprocess_force_exit(br->proc);

  /* Optional diagnostic routed as "stderr" to the sink. */
  append_line(br, "[runner] stop requested", TRUE);
}

/*=============================================================================
 * Internal helpers (static)
 *===========================================================================*/

/*------------------------------------------------------------------------------
 * append_line:
 *   Tiny helper that guards the sink and never passes NULL strings through.
 *----------------------------------------------------------------------------*/
static void
append_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
  if (br && br->sink) {
    br->sink(br->sink_user, s ? s : "", is_err);
  }
}

/*------------------------------------------------------------------------------
 * Schedule the next async read on STDOUT (line-oriented).
 *----------------------------------------------------------------------------*/
static void
read_next_stdout(UmiBuildRunner *br)
{
  if (!br || !br->out_r) return;
  g_data_input_stream_read_line_async(
      br->out_r,                  /* stream to read from                      */
      G_PRIORITY_DEFAULT,         /* priority                                 */
      br->cancel,                 /* cancellable (can be NULL)                */
      on_read_stdout,             /* completion callback                      */
      br                          /* user data to callback                    */
  );
}

/*------------------------------------------------------------------------------
 * Schedule the next async read on STDERR (line-oriented).
 *----------------------------------------------------------------------------*/
static void
read_next_stderr(UmiBuildRunner *br)
{
  if (!br || !br->err_r) return;
  g_data_input_stream_read_line_async(
      br->err_r,
      G_PRIORITY_DEFAULT,
      br->cancel,
      on_read_stderr,
      br
  );
}

/*------------------------------------------------------------------------------
 * on_read_stdout:
 *   Completion for a single line read from STDOUT. Emits the line to the sink
 *   and immediately schedules the next read. EOF is signaled by (line == NULL)
 *   with no error; in that case we simply stop rescheduling.
 *----------------------------------------------------------------------------*/
static void
on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  gsize   len  = 0;                 /* receives line length (unused)           */
  GError *gerr = NULL;              /* receives read error if any              */
  gchar  *line = g_data_input_stream_read_line_finish(
                    G_DATA_INPUT_STREAM(src), res, &len, &gerr);

  if (gerr) {                       /* report the read error as stderr line    */
    append_line(br, gerr->message, TRUE);
    g_error_free(gerr);
    return;                         /* do not chain next read on error         */
  }

  if (line) {                       /* got a line => forward and continue      */
    append_line(br, line, FALSE);
    g_free(line);
    read_next_stdout(br);
  }
  /* line == NULL and no error => EOF: do nothing (stop chaining). */
}

/*------------------------------------------------------------------------------
 * on_read_stderr:
 *   Symmetric to on_read_stdout but marks lines as "is_err = TRUE".
 *----------------------------------------------------------------------------*/
static void
on_read_stderr(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  gsize   len  = 0;
  GError *gerr = NULL;
  gchar  *line = g_data_input_stream_read_line_finish(
                    G_DATA_INPUT_STREAM(src), res, &len, &gerr);

  if (gerr) {
    append_line(br, gerr->message, TRUE);
    g_error_free(gerr);
    return;
  }

  if (line) {
    append_line(br, line, TRUE);
    g_free(line);
    read_next_stderr(br);
  }
  /* EOF: stop chaining. */
}

/*------------------------------------------------------------------------------
 * on_proc_exited:
 *   Completion for g_subprocess_wait_async(). If the wait itself fails we
 *   surface the error text via the sink. Otherwise we retrieve the exit code
 *   and notify the on_exit callback (if present).
 *----------------------------------------------------------------------------*/
static void
on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  GError *gerr = NULL;
  if (!g_subprocess_wait_finish(G_SUBPROCESS(src), res, &gerr)) {
    if (gerr) {                     /* Uncommon, but propagate for visibility  */
      append_line(br, gerr->message, TRUE);
      g_error_free(gerr);
    }
  }

  /* Notify exit status to the caller if they asked for it. */
  if (br->on_exit) {
    int code = g_subprocess_get_exit_status(G_SUBPROCESS(src));
    br->on_exit(br->user, code);
  }
}
/*-----------------------------------------------------------------------------
 * End of src/build/build_runner.c
 *---------------------------------------------------------------------------*/
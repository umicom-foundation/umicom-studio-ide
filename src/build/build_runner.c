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
 * PUBLIC API (must match src/build/include/build_runner.h exactly):
 *
 *   // Construction / lifetime ------------------------------------------------
 *   UmiBuildRunner *umi_build_runner_new(void);
 *   void            umi_build_runner_free(UmiBuildRunner *br);
 *
 *   // Output streaming (optional) -------------------------------------------
 *   // Lines are delivered as UTF-8 text, one logical line per callback.
 *   // is_err = FALSE for stdout, TRUE for stderr.
 *   void            umi_build_runner_set_sink(UmiBuildRunner *br,
 *                                             UmiOutputSink   sink,
 *                                             gpointer        sink_user);
 *
 *   // Execution --------------------------------------------------------------
 *   // Launch argv[0] with argv/envp/cwd. All parameters follow GLib
 *   // conventions (NULL-terminated string vectors; NULL envp => inherit).
 *   // Returns TRUE on successful spawn, otherwise FALSE and sets @err.
 *   gboolean        umi_build_runner_run(UmiBuildRunner   *br,
 *                                        char * const     *argv,
 *                                        char * const     *envp,
 *                                        const char       *cwd,
 *                                        UmiBuildExitCb    on_exit,
 *                                        gpointer          user,
 *                                        GError          **err);
 *
 *   // Best-effort termination -----------------------------------------------
 *   // Cancels outstanding async reads and forces the subprocess to exit.
 *   void            umi_build_runner_stop(UmiBuildRunner *br);
 *
 * DESIGN NOTES:
 *   - Opaque runner type; callers cannot touch internals.
 *   - Nonblocking I/O: line-oriented reads via GDataInputStream async API.
 *   - Output sink is optional; when set, every line goes through it.
 *   - Exit notification via UmiBuildExitCb(user, exit_code).
 *   - No GTK dependency; pure GLib/GIO here.
 *
 * SAFETY & OWNERSHIP:
 *   - All GObjects are released with g_clear_object().
 *   - Lines returned by GLib are freed with g_free().
 *   - Environment vectors are duplicated and freed (no const-cast warnings).
 *
 * THREADING:
 *   - Intended for main-thread use. All callbacks are dispatched on the GLib
 *     main context associated with the thread that created the objects.
 *
 * COMPAT:
 *   - C11 + GLib 2.7x+. Windows/Unix supported via GSubprocess.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"            /* Public contract for this module          */
#include <gio/gio.h>                 /* GSubprocess, GDataInputStream, etc.     */
#include <string.h>                  /* memset/strcmp helpers (if ever needed)  */

/*=============================================================================
 * File-local types and helpers
 *===========================================================================*/

/* Forward declarations for internal helpers. These are static on purpose:
 * nothing here should leak as symbol outside this translation unit.         */
static void         read_next_stdout   (struct _UmiBuildRunner *br);
static void         read_next_stderr   (struct _UmiBuildRunner *br);
static void         append_line        (struct _UmiBuildRunner *br,
                                        const char *s,
                                        gboolean    is_err);
static void         on_read_stdout     (GObject      *src,
                                        GAsyncResult *res,
                                        gpointer      user);
static void         on_read_stderr     (GObject      *src,
                                        GAsyncResult *res,
                                        gpointer      user);
static void         on_proc_exited     (GObject      *src,
                                        GAsyncResult *res,
                                        gpointer      user);

/*------------------------------------------------------------------------------
 * dup_envp:
 *   Duplicate a NULL-terminated env vector in a const-correct way.
 *
 * WHY:
 *   g_strdupv() takes (gchar **). If callers pass a const-qualified vector
 *   (e.g., `char * const *envp`), casting to `gchar **` raises the warning
 *   you saw: “discarded 'const' qualifier from pointer target type”.
 *   This tiny helper avoids the cast entirely and keeps the compiler happy.
 *
 * RETURNS:
 *   Newly allocated `char **` with copies of each string + trailing NULL.
 *   Free with g_strfreev().
 *----------------------------------------------------------------------------*/
static char **dup_envp(char * const *envp)
{
    if (!envp) return NULL;                 /* NULL => caller inherits          */

    /* Count entries safely. */
    size_t n = 0;
    for (char * const *p = envp; *p; ++p) n++;

    /* Allocate (n + 1) slots; g_new0() pre-NULLs the terminator. */
    char **out = g_new0(char *, n + 1);

    /* Duplicate each "KEY=VALUE" string. */
    for (size_t i = 0; i < n; ++i) {
        out[i] = g_strdup(envp[i]);         /* GLib strdup (handles NULL safe)  */
    }
    /* out[n] is already NULL. */
    return out;
}

/*------------------------------------------------------------------------------
 * The private runner struct (opaque outside this file).
 *   - Holds the child process and the line-readers for stdout/stderr.
 *   - Tracks the sink and exit callback context provided by the caller.
 *----------------------------------------------------------------------------*/
struct _UmiBuildRunner {
    /* Child process + I/O readers ------------------------------------------ */
    GSubprocess       *proc;        /* running child process (or NULL)        */
    GDataInputStream  *out_r;       /* line-reader wrapping child's STDOUT     */
    GDataInputStream  *err_r;       /* line-reader wrapping child's STDERR     */
    GCancellable      *cancel;      /* cancels pending async operations        */

    /* Callbacks supplied by the caller ------------------------------------- */
    UmiBuildExitCb     on_exit;     /* called when process exits               */
    gpointer           user;        /* opaque user pointer for on_exit         */

    /* Optional sink for each output line ----------------------------------- */
    UmiOutputSink      sink;        /* (sink_user, line, is_err)               */
    gpointer           sink_user;   /* opaque pointer forwarded to the sink    */
};

/*=============================================================================
 * Public API
 *===========================================================================*/

/*------------------------------------------------------------------------------
 * umi_build_runner_new:
 *   Allocate a runner instance. All fields are zero-initialized.
 *   A dedicated GCancellable is created for easy stop/cancel semantics.
 *----------------------------------------------------------------------------*/
UmiBuildRunner *
umi_build_runner_new(void)
{
    UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);   /* zero-init all fields */
    br->cancel = g_cancellable_new();                 /* ready for async ops  */
    return br;                                        /* caller owns it       */
}

/*------------------------------------------------------------------------------
 * umi_build_runner_free:
 *   Destroy the runner and release all resources. Safe to pass NULL.
 *   If any async I/O is still in flight, we cancel first to unwind cleanly.
 *----------------------------------------------------------------------------*/
void
umi_build_runner_free(UmiBuildRunner *br)
{
    if (!br) return;                                  /* guard against NULL   */

    /* Cancel any pending I/O; callbacks will observe cancellation. */
    if (br->cancel) g_cancellable_cancel(br->cancel);

    /* Release in reverse order of creation to be polite. */
    g_clear_object(&br->out_r);
    g_clear_object(&br->err_r);
    g_clear_object(&br->proc);
    g_clear_object(&br->cancel);

    /* Finally free the struct itself. */
    g_free(br);
}

/*------------------------------------------------------------------------------
 * umi_build_runner_set_sink:
 *   Register (or clear) the per-line output sink. The sink is invoked from the
 *   GLib main context whenever a line is read from stdout or stderr.
 *----------------------------------------------------------------------------*/
void
umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user)
{
    if (!br) return;
    br->sink      = sink;
    br->sink_user = sink_user;
}

/*------------------------------------------------------------------------------
 * umi_build_runner_run:
 *   Spawn the process, set up line-oriented async reads on stdout/stderr,
 *   and request an async wait for exit. We never block the caller.
 *
 * RETURNS:
 *   TRUE on success (child spawned), FALSE with @err set otherwise.
 *----------------------------------------------------------------------------*/
gboolean
umi_build_runner_run(UmiBuildRunner  *br,
                     char * const    *argv,      /* NULL-terminated vector    */
                     char * const    *envp,      /* NULL => inherit           */
                     const char      *cwd,       /* NULL => inherit           */
                     UmiBuildExitCb   on_exit,   /* optional                  */
                     gpointer         user,      /* user data for on_exit     */
                     GError         **err)       /* out-param for errors      */
{
    /* Validate early for clearer diagnostics. */
    if (!br || !argv || !argv[0]) {
        g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                    "umi_build_runner_run: invalid arguments (runner/argv)");
        return FALSE;
    }

    /* Record exit callback context (may be NULL). */
    br->on_exit = on_exit;
    br->user    = user;

    /* Create a launcher capturing stdout/stderr as pipes. */
    GSubprocessLauncher *launch =
        g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                  G_SUBPROCESS_FLAGS_STDERR_PIPE);

    /* Apply working directory if requested. */
    if (cwd && *cwd) {
        g_subprocess_launcher_set_cwd(launch, cwd);
    }

    /* Handle environment consistently:
     *   - If envp == NULL: inherit the current process environment.
     *   - Otherwise: duplicate the provided envp and set it on the launcher.
     * We avoid const-casts and free our temp copies immediately after.       */
    if (envp) {
        char **dup = dup_envp((char * const *)envp);      /* const-correct   */
        g_subprocess_launcher_set_environ(launch, dup);
        g_strfreev(dup);
    } else {
        char **inherit = g_get_environ();                 /* snapshot        */
        g_subprocess_launcher_set_environ(launch, inherit);
        g_strfreev(inherit);
    }

    /* Spawn the child process with argv. GLib copies the vector, so the
     * lifetime of argv is purely the caller's concern.                      */
    GError *spawn_err = NULL;
    br->proc = g_subprocess_launcher_spawnv(
                    launch,
                    (const gchar * const *)argv,          /* read-only view  */
                    &spawn_err);

    /* Launcher no longer needed regardless of outcome. */
    g_object_unref(launch);

    /* Bail out if spawn failed; nothing allocated beyond the launcher yet. */
    if (!br->proc) {
        if (spawn_err) g_propagate_error(err, spawn_err);
        return FALSE;
    }

    /* Wrap stdout/stderr with line-readers. We keep br->cancel attached to
     * all async operations to make umi_build_runner_stop() responsive.      */
    GInputStream *out_s = g_subprocess_get_stdout_pipe(br->proc);
    GInputStream *err_s = g_subprocess_get_stderr_pipe(br->proc);
    br->out_r = g_data_input_stream_new(out_s);
    br->err_r = g_data_input_stream_new(err_s);

    /* Kick off the first async reads; each completion schedules the next. */
    read_next_stdout(br);
    read_next_stderr(br);

    /* Arrange for non-blocking exit notification. */
    g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);

    return TRUE;                                     /* successfully launched */
}

/*------------------------------------------------------------------------------
 * umi_build_runner_stop:
 *   Best-effort termination sequence:
 *     1) Cancel pending async I/O (read_line_async) to unwind callbacks.
 *     2) Force the child to exit (works across platforms).
 *     3) Emit a diagnostic line through the sink (if present).
 *----------------------------------------------------------------------------*/
void
umi_build_runner_stop(UmiBuildRunner *br)
{
    if (!br) return;

    /* Cancel all pending operations early. */
    if (br->cancel) g_cancellable_cancel(br->cancel);

    /* Force the subprocess to exit if it’s still alive. */
    if (br->proc) g_subprocess_force_exit(br->proc);

    /* Optional diagnostic routed as “stderr”. */
    append_line(br, "[runner] stop requested", TRUE);
}

/*=============================================================================
 * Internal helpers (static)
 *===========================================================================*/

/*------------------------------------------------------------------------------
 * append_line:
 *   Push a line into the configured sink if present. We defensively translate
 *   NULL into an empty string to avoid surprising callers.
 *----------------------------------------------------------------------------*/
static void
append_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
    if (br && br->sink) {
        br->sink(br->sink_user, s ? s : "", is_err);
    }
}

/*------------------------------------------------------------------------------
 * read_next_stdout / read_next_stderr:
 *   Schedule a single line-oriented async read on the respective stream. On
 *   completion, the *_finish handler will re-schedule the next read until EOF.
 *----------------------------------------------------------------------------*/
static void
read_next_stdout(UmiBuildRunner *br)
{
    if (!br || !br->out_r) return;

    g_data_input_stream_read_line_async(
        br->out_r,                      /* stream to read from                */
        G_PRIORITY_DEFAULT,             /* typical priority                   */
        br->cancel,                     /* may be NULL, but we have one       */
        on_read_stdout,                 /* completion callback                */
        br                               /* user data                          */
    );
}

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
 *   Handle completion for a stdout line. If a line is produced, forward it
 *   and immediately schedule the next read. On error, report via sink and stop
 *   chaining further reads (the cancel/stop path will clean up).
 *----------------------------------------------------------------------------*/
static void
on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
    UmiBuildRunner *br = (UmiBuildRunner *)user;

    gsize   len  = 0;                         /* length is informational only  */
    GError *gerr = NULL;
    gchar  *line = g_data_input_stream_read_line_finish(
                        G_DATA_INPUT_STREAM(src), res, &len, &gerr);

    if (gerr) {
        append_line(br, gerr->message, TRUE); /* surface the read error        */
        g_error_free(gerr);
        return;                               /* do not reschedule on error    */
    }

    if (line) {
        append_line(br, line, FALSE);         /* stdout line                   */
        g_free(line);
        read_next_stdout(br);                 /* chain the next read           */
    }
    /* line == NULL (EOF) => stop chaining. */
}

/*------------------------------------------------------------------------------
 * on_read_stderr:
 *   Same as on_read_stdout but marks the line as coming from stderr.
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
        append_line(br, line, TRUE);          /* stderr line                   */
        g_free(line);
        read_next_stderr(br);
    }
    /* EOF => stop chaining. */
}

/*------------------------------------------------------------------------------
 * on_proc_exited:
 *   Completion callback for g_subprocess_wait_async(). We translate any wait
 *   error to the sink (rare), then notify the caller via on_exit() with the
 *   process exit status. We purposely do not free here; the runner is owned by
 *   the caller, who decides when to free it (often after on_exit runs).
 *----------------------------------------------------------------------------*/
static void
on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
    UmiBuildRunner *br = (UmiBuildRunner *)user;

    GError *gerr = NULL;
    if (!g_subprocess_wait_finish(G_SUBPROCESS(src), res, &gerr)) {
        if (gerr) {
            append_line(br, gerr->message, TRUE);
            g_error_free(gerr);
        }
    }

    if (br->on_exit) {
        int code = g_subprocess_get_exit_status(G_SUBPROCESS(src));
        br->on_exit(br->user, code);
    }
}

/*-----------------------------------------------------------------------------
 * End of src/build/build_runner.c
 *---------------------------------------------------------------------------*/

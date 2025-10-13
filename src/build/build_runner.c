/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 *
 * PURPOSE:
 *   Concrete implementation of UmiBuildRunner. Spawns an external process
 *   (e.g., ninja/make/ctest), reads stdout/stderr line-by-line, and forwards
 *   them to an injected UmiOutputSink. Reports final exit status via callback.
 *
 * KEY POINTS:
 *   - This module *never* includes UI or pane headers. It depends only on
 *     umi_output_sink.h which is a small decoupled interface.
 *   - Uses non-blocking async reads (GDataInputStream + read_line_async)
 *     to integrate naturally with the GLib main loop.
 *   - Environment handling: GLib expects a writable char** for custom env,
 *     so we duplicate the provided envp (if any) and free our copy.
 *
 * WARNINGS ELIMINATED:
 *   - No nested comment sequences in docstrings (avoid "/*" patterns).
 *   - No 'discarded-qualifiers' warnings when passing env via launcher:
 *     we call g_strdupv() to produce a writable environment vector.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"

#include <string.h>   /* g_strcmp0, etc.                                               */

/*-----------------------------------------------------------------------------
 * Private structure — not exposed in the header (keeps ABI/API clean).
 *---------------------------------------------------------------------------*/
struct _UmiBuildRunner
{
    UmiOutputSink     *sink;        /* where lines go; may be NULL                   */
    GSubprocess       *proc;        /* the running process                           */
    GDataInputStream  *out_r;       /* line reader for stdout                        */
    GDataInputStream  *err_r;       /* line reader for stderr                        */
    GCancellable      *cancel;      /* cancel async I/O and waits                    */

    void (*on_exit)(gpointer user, int code); /* exit callback (optional)            */
    gpointer           user;        /* exit callback user-data                       */
};

/*-----------------------------------------------------------------------------
 * Forward declarations of small helpers (file-local).
 *---------------------------------------------------------------------------*/
static void sink_line(UmiBuildRunner *br, const char *s, gboolean is_err);
static void read_next_stdout(UmiBuildRunner *br);
static void read_next_stderr(UmiBuildRunner *br);

/* Async completion callbacks for read_line_async() */
static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user);
static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user);

/* Subprocess exit callback */
static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user);

/*-----------------------------------------------------------------------------
 * Construction / Destruction
 *---------------------------------------------------------------------------*/
UmiBuildRunner *umi_build_runner_new(void)
{
    UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);   /* zero-init all fields */
    br->cancel = g_cancellable_new();
    return br;
}

void umi_build_runner_free(UmiBuildRunner *br)
{
    if (!br) return;

    /* Try to stop background activity first. */
    if (br->cancel) g_cancellable_cancel(br->cancel);

    g_clear_object(&br->out_r);
    g_clear_object(&br->err_r);
    g_clear_object(&br->proc);
    g_clear_object(&br->cancel);
    /* 'sink' is owned by caller; do not free here. */

    g_free(br);
}

/*-----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink *sink)
{
    if (!br) return;
    br->sink = sink;
}

/*-----------------------------------------------------------------------------
 * Run a process, wire pipes, begin async reads, notify when done.
 *---------------------------------------------------------------------------*/
gboolean umi_build_runner_run(UmiBuildRunner *br,
                              char * const   *argv,
                              char * const   *envp,
                              const char     *cwd,
                              void (*on_exit)(gpointer user, int code),
                              gpointer        user)
{
    if (!br || !argv || !argv[0]) return FALSE;

    /* Remember completion callback */
    br->on_exit = on_exit;
    br->user    = user;

    /* Launcher configured to capture stdout/stderr as pipes */
    GSubprocessLauncher *ln = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                                        G_SUBPROCESS_FLAGS_STDERR_PIPE);
    if (!ln) return FALSE;

    if (cwd && *cwd) g_subprocess_launcher_set_cwd(ln, cwd);

    /* GLib expects a non-const env (char**). If caller provided an envp,
     * duplicate it into writable memory (we free our copy after spawn).     */
    gchar **dup_env = NULL;
    if (envp) { dup_env = g_strdupv((gchar **)envp); }
    if (dup_env) { g_subprocess_launcher_set_environ(ln, dup_env); }

    /* Spawn the process (argv must be NULL-terminated). */
    GError *err = NULL;
    br->proc = g_subprocess_launcher_spawnv(ln, (const gchar * const *)argv, &err);
    g_object_unref(ln);
    if (dup_env) g_strfreev(dup_env);

    if (!br->proc)
    {
        sink_line(br, "[runner] failed to start process", TRUE);
        if (err) { sink_line(br, err->message ? err->message : "unknown error", TRUE); g_clear_error(&err); }
        return FALSE;
    }

    /* Own readers for stdout / stderr */
    GInputStream *out_s = g_subprocess_get_stdout_pipe(br->proc);
    GInputStream *err_s = g_subprocess_get_stderr_pipe(br->proc);
    br->out_r = g_data_input_stream_new(out_s);
    br->err_r = g_data_input_stream_new(err_s);

    /* Kick off non-blocking reads */
    read_next_stdout(br);
    read_next_stderr(br);

    /* Wait for exit asynchronously */
    g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);

    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Stop a running process (best-effort).
 *---------------------------------------------------------------------------*/
void umi_build_runner_stop(UmiBuildRunner *br)
{
    if (!br) return;

    if (br->cancel) g_cancellable_cancel(br->cancel);
    if (br->proc)   g_subprocess_force_exit(br->proc);

    sink_line(br, "[runner] stop requested", TRUE);
}

/*-----------------------------------------------------------------------------
 * Helpers
 *---------------------------------------------------------------------------*/
static void sink_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
    if (br && br->sink && br->sink->on_line)
        br->sink->on_line(br->sink->user, s ? s : "", is_err);
}

static void read_next_stdout(UmiBuildRunner *br)
{
    if (!br || !br->out_r) return;
    g_data_input_stream_read_line_async(br->out_r, G_PRIORITY_DEFAULT, br->cancel,
                                        on_read_stdout, br);
}

static void read_next_stderr(UmiBuildRunner *br)
{
    if (!br || !br->err_r) return;
    g_data_input_stream_read_line_async(br->err_r, G_PRIORITY_DEFAULT, br->cancel,
                                        on_read_stderr, br);
}

/*-----------------------------------------------------------------------------
 * Async completion handlers
 *---------------------------------------------------------------------------*/
static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
    UmiBuildRunner *br = user;
    gsize len = 0;
    GError *err = NULL;

    char *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &err);
    if (err) { sink_line(br, err->message ? err->message : "stdout read error", TRUE); g_clear_error(&err); }
    if (line) { sink_line(br, line, FALSE); g_free(line); }

    /* Schedule next read (unless canceled) */
    if (!g_cancellable_is_cancelled(br ? br->cancel : NULL))
        read_next_stdout(br);
}

static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user)
{
    UmiBuildRunner *br = user;
    gsize len = 0;
    GError *err = NULL;

    char *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &err);
    if (err) { sink_line(br, err->message ? err->message : "stderr read error", TRUE); g_clear_error(&err); }
    if (line) { sink_line(br, line, TRUE); g_free(line); }

    if (!g_cancellable_is_cancelled(br ? br->cancel : NULL))
        read_next_stderr(br);
}

/*-----------------------------------------------------------------------------
 * Process exit
 *---------------------------------------------------------------------------*/
static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
    UmiBuildRunner *br = user;
    GError *err = NULL;
    gboolean ok = g_subprocess_wait_finish(G_SUBPROCESS(src), res, &err);
    if (err) { sink_line(br, err->message ? err->message : "wait error", TRUE); g_clear_error(&err); }

    int code = ok ? g_subprocess_get_exit_status(G_SUBPROCESS(src)) : -1;
    if (br && br->on_exit) br->on_exit(br->user, code);
}

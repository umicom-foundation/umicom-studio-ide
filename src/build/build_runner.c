/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 * PURPOSE: Launches external build processes using GIO (GSubprocess),
 *          streams stdout/stderr asynchronously, and notifies on exit.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"         /* <-- Always include our own header first to ensure
                                   *     our public types (UmiBuildRunner, UmiBuildExitCb)
                                   *     are visible and consistent. */

/* GIO/GLib headers provide the types and APIs we use here:
 * - GSubprocess / GSubprocessLauncher for running processes
 * - GDataInputStream for async line reads from stdout/stderr
 * - GCancellable for cooperative cancellation
 * - GAsyncResult / GError for async completion + error reporting
 * - GLib base types like gboolean, gpointer, g_free, etc.
 */
#include <gio/gio.h>              /* <-- REQUIRED: GSubprocess, GDataInputStream, GAsync* */
#include <glib.h>                 /* <-- REQUIRED: gboolean, gpointer, g_new0, g_free, etc. */

/*-----------------------------------------------------------------------------
 * Internal helpers forward-declared here (file-local). We declare them before
 * use so the compiler knows their signatures.
 *---------------------------------------------------------------------------*/

/* Reads the next stdout line asynchronously. */
static void read_next_stdout(struct _UmiBuildRunner *br);
/* Reads the next stderr line asynchronously. */
static void read_next_stderr(struct _UmiBuildRunner *br);

/* Append one line to the connected sink (Output Pane / adapter); 'is_err' flags stderr. */
static void append_line(struct _UmiBuildRunner *br, const char *s, gboolean is_err);

/* Async completion handlers for stdout/stderr reads and process-exit. */
static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user);
static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user);
static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user);

/*-----------------------------------------------------------------------------
 * UmiBuildRunner definition (kept private to this .c to hide internals).
 * The public opaque struct is forward-declared in build_runner.h.
 *---------------------------------------------------------------------------*/
struct _UmiBuildRunner {
  GSubprocess       *proc;        /* Handle to the child process. */
  GDataInputStream  *out_r;       /* Async line reader for stdout. */
  GDataInputStream  *err_r;       /* Async line reader for stderr. */
  GCancellable      *cancel;      /* Allows cooperative cancellation. */
  UmiBuildExitCb     on_exit;     /* User-provided callback for process exit. */
  gpointer           user;        /* Opaque user pointer passed back on exit. */

  /* Optional sink interface (kept generic). For now we use the adapter in the
   * header; if NULL, we can silently drop or route to a default logger. */
  UmiOutputSink      sink;        /* Function pointer to append lines. */
  gpointer           sink_user;   /* User-data for sink. */
};

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

UmiBuildRunner *umi_build_runner_new(void)
{
  /* Allocate and zero-initialize runner. g_new0 ensures all pointers are NULL. */
  UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);

  /* Create a cancellable so we can stop async operations and the process. */
  br->cancel = g_cancellable_new();

  return br;
}

void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user)
{
  /* Store the sink (callback) and its user data; either may be NULL if caller
   * wants to disable output routing. */
  br->sink = sink;
  br->sink_user = sink_user;
}

gboolean umi_build_runner_run(UmiBuildRunner *br,
                              char        * const *argv,
                              char        * const *envp,
                              const char         *cwd,
                              UmiBuildExitCb      on_exit,
                              gpointer            user,
                              GError            **err)
{
  /* Validate inputs early. argv must contain a program. */
  if (!br || !argv || !argv[0]) {
    g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                "umi_build_runner_run: invalid arguments (runner/argv)");
    return FALSE;
  }

  /* Save the exit callback + user for later notification. */
  br->on_exit = on_exit;
  br->user    = user;

  /* Configure subprocess: we want stdout/stderr as pipes to read lines. */
  GSubprocessLauncher *launch = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                                          G_SUBPROCESS_FLAGS_STDERR_PIPE);
  /* If a working directory is provided, set it. */
  if (cwd && *cwd) {
    g_subprocess_launcher_set_cwd(launch, cwd);
  }

  /* If envp was provided, apply it; otherwise inherit. */
  if (envp) {
    /* Replace the environment with envp (NULL-terminated). */
    g_subprocess_launcher_set_environ(launch, (char * const *)envp);
  }

  /* Spawn the process. On Windows, this works fine under MSYS2/MinGW. */
  br->proc = g_subprocess_launcher_spawnv(launch, (const gchar * const *)argv, err);

  /* Launcher no longer needed once we've spawned (or failed). */
  g_object_unref(launch);

  /* If spawn failed, return FALSE with *err already set. */
  if (!br->proc) {
    return FALSE;
  }

  /* Wrap stdout/stderr streams with GDataInputStream to read lines easily. */
  GInputStream *out_stream = g_subprocess_get_stdout_pipe(br->proc);
  GInputStream *err_stream = g_subprocess_get_stderr_pipe(br->proc);

  br->out_r = g_data_input_stream_new(out_stream);
  br->err_r = g_data_input_stream_new(err_stream);

  /* Start reading both streams asynchronously. */
  read_next_stdout(br);
  read_next_stderr(br);

  /* When the process exits, we want to be notified asynchronously. */
  g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);

  /* Success. */
  return TRUE;
}

void umi_build_runner_stop(UmiBuildRunner *br)
{
  /* Gracefully cancel any pending async reads and request process exit. */
  if (!br) return;

  g_cancellable_cancel(br->cancel);      /* Cancels pending async ops. */
  g_subprocess_force_exit(br->proc);     /* Ask the child to exit if still running. */
  append_line(br, "[runner] stop requested", TRUE);  /* Inform the sink (stderr). */
}

void umi_build_runner_free(UmiBuildRunner *br)
{
  if (!br) return;

  /* Drop our stream readers. */
  g_clear_object(&br->out_r);
  g_clear_object(&br->err_r);

  /* Drop the process and cancellable. */
  g_clear_object(&br->proc);
  g_clear_object(&br->cancel);

  /* Finally free the struct itself. */
  g_free(br);
}

/*-----------------------------------------------------------------------------
 * Internal helpers (file-local)
 *---------------------------------------------------------------------------*/

static void append_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
  /* Route to sink if configured; otherwise we silently ignore.
   * You could also route to a fallback logger here. */
  if (br && br->sink) {
    br->sink(br->sink_user, s ? s : "", is_err);
  }
}

/* Kick off an async line read from stdout. */
static void read_next_stdout(UmiBuildRunner *br)
{
  if (!br || !br->out_r) return;

  g_data_input_stream_read_line_async(
      br->out_r,                 /* the data input stream (stdout) */
      G_PRIORITY_DEFAULT,        /* scheduling priority */
      br->cancel,                /* cancellable for graceful shutdown */
      on_read_stdout,            /* completion callback */
      br                         /* user data passed to callback */
  );
}

/* Kick off an async line read from stderr. */
static void read_next_stderr(UmiBuildRunner *br)
{
  if (!br || !br->err_r) return;

  g_data_input_stream_read_line_async(
      br->err_r,                 /* the data input stream (stderr) */
      G_PRIORITY_DEFAULT,        /* scheduling priority */
      br->cancel,                /* cancellable for graceful shutdown */
      on_read_stderr,            /* completion callback */
      br                         /* user data passed to callback */
  );
}

/* Called each time a stdout line is ready. */
static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  /* Extract line; GLib allocates it for us. */
  gsize len = 0;
  GError *err = NULL;
  gchar *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &err);

  if (err) {
    /* Report error to sink. */
    append_line(br, err->message, TRUE);
    g_error_free(err);
    return;
  }

  if (line) {
    /* Deliver the line, then free it. */
    append_line(br, line, FALSE);
    g_free(line);

    /* Continue reading next line. */
    read_next_stdout(br);
  }
  /* If line is NULL and no error: EOF reached; do nothing. */
}

/* Called each time a stderr line is ready. */
static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  gsize len = 0;
  GError *err = NULL;
  gchar *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &err);

  if (err) {
    append_line(br, err->message, TRUE);
    g_error_free(err);
    return;
  }

  if (line) {
    append_line(br, line, TRUE);   /* Mark as stderr. */
    g_free(line);
    read_next_stderr(br);
  }
}

/* Called when the process exits. */
static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;

  GError *err = NULL;
  gboolean ok = g_subprocess_wait_finish(G_SUBPROCESS(src), res, &err);

  int exit_code = -1;
  if (ok) {
    /* Extract exit status. */
    if (g_subprocess_get_if_exited(G_SUBPROCESS(src)))
      exit_code = g_subprocess_get_exit_status(G_SUBPROCESS(src));
  } else if (err) {
    /* Propagate an error message to the sink and use -1 as code. */
    append_line(br, err->message, TRUE);
    g_error_free(err);
  }

  /* Notify caller if they registered a callback. */
  if (br->on_exit) {
    br->on_exit(br->user, exit_code);
  }
}

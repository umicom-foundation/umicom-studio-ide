/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 *
 * PROJECT:
 *   Umicom Studio IDE — build subsystem
 *
 * PURPOSE:
 *   Spawn external build processes (compiler, linker, tools) using GLib/GIO
 *   GSubprocess; stream stdout/stderr asynchronously into a caller-supplied
 *   sink; notify the caller when the child exits.
 *
 * DESIGN:
 *   - Pure C (C11) + GLib/GIO; no cross-module includes beyond our own header.
 *   - Opaque UmiBuildRunner type (internals hidden here → loose coupling).
 *   - Asynchronous line reads from stdout/stderr with cancellation support.
 *   - Optional “output sink” callback for incremental UI logging.
 *   - Optional exit callback fired once on process termination.
 *
 * SAFETY:
 *   - Never touch GTK from worker/IO callbacks; only call sink callback the
 *     caller gave us (which must marshal back to main thread if needed).
 *   - Avoid dangling pointers by owning all GLib objects with g_clear_object().
 *   - Avoid buffer overflow: GLib allocates lines, we free with g_free().
 *
 * REQUIREMENTS:
 *   - GLib (gio-2.0, glib-2.0) available at build/link time.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab
 * Date: 2025-10-12 | License: MIT
 *---------------------------------------------------------------------------*/

#include "build_runner.h"   /* Public API: types and function prototypes.   */
#include <gio/gio.h>        /* GSubprocess, GDataInputStream, GCancellable. */
#include <glib.h>           /* Basics: gboolean, g_autoptr, g_new0, etc.    */

/*-----------------------------------------------------------------------------
 * INTERNAL OVERVIEW (PSEUDOCODE)
 *
 *   struct Runner {
 *     GSubprocess*      proc;     // child process handle
 *     GDataInputStream* out_r;    // async line reader for stdout
 *     GDataInputStream* err_r;    // async line reader for stderr
 *     GCancellable*     cancel;   // cancels async IO + wait
 *     UmiOutputSink     sink;     // (user) line sink callback
 *     gpointer          sink_user;// sink callback user pointer
 *     UmiBuildExitCb    on_exit;  // exit callback
 *     gpointer          user;     // exit callback user pointer
 *   }
 *
 *   run():
 *     launcher = g_subprocess_launcher_new(PIPE_OUT|PIPE_ERR)
 *     if (cwd) set_cwd(launcher, cwd)
 *     if (envp) {
 *       dup = g_strdupv(envp)          // make a mutable copy
 *       g_subprocess_launcher_set_environ(launcher, dup)   // GLib consumes
 *       // do NOT free dup — ownership transferred to launcher
 *     }
 *     proc = spawnv(launcher, argv)
 *     out_r = new data stream from stdout; read_line_async(...)
 *     err_r = new data stream from stderr; read_line_async(...)
 *     wait_async(proc, on_proc_exited)
 *
 *   IO callbacks (stdout/stderr):
 *     line = finish_read_line(...)
 *     if (line) sink(line); free(line); read_line_async(...) again
 *
 *   on_proc_exited():
 *     code = exit status or -1 on error
 *     if (on_exit) on_exit(user, code)
 *---------------------------------------------------------------------------*/

/* Forward declarations for our internal helpers. Each is file-local (static). */
typedef struct _UmiBuildRunner UmiBuildRunner;  /* forward for prototypes     */
static void read_next_stdout(UmiBuildRunner *br);     /* arm async read (out)   */
static void read_next_stderr(UmiBuildRunner *br);     /* arm async read (err)   */
static void append_line   (UmiBuildRunner *br, const char *s, gboolean is_err);
static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer user);
static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer user);
static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer user);

/* Hide the struct internals here to keep the header self-contained.          */
struct _UmiBuildRunner {
  GSubprocess      *proc;        /* child process handle                      */
  GDataInputStream *out_r;       /* async line reader for stdout              */
  GDataInputStream *err_r;       /* async line reader for stderr              */
  GCancellable     *cancel;      /* cancels reads + wait                      */
  UmiBuildExitCb    on_exit;     /* optional “process exited” callback        */
  gpointer          user;        /* user data for on_exit                     */
  UmiOutputSink     sink;        /* optional output sink callback             */
  gpointer          sink_user;   /* user data for sink                        */
};

/*--------------------------- PUBLIC API: new/free ---------------------------*/

UmiBuildRunner *
umi_build_runner_new(void)
{
  /* Allocate zero-initialized runner (all pointers start as NULL).           */
  UmiBuildRunner *br = g_new0(UmiBuildRunner, 1);       /* allocate struct   */
  br->cancel = g_cancellable_new();                     /* create cancellable*/
  return br;                                            /* return handle     */
}

void
umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user)
{
  /* Store the sink callback and its user pointer (both may be NULL).         */
  br->sink      = sink;                                  /* save callback     */
  br->sink_user = sink_user;                             /* save context      */
}

void
umi_build_runner_free(UmiBuildRunner *br)
{
  if (!br) return;                                       /* nullptr-safe      */
  g_clear_object(&br->out_r);                            /* drop stdout reader*/
  g_clear_object(&br->err_r);                            /* drop stderr reader*/
  g_clear_object(&br->proc);                             /* drop child handle */
  g_clear_object(&br->cancel);                           /* drop cancellable  */
  g_free(br);                                            /* free struct       */
}

/*--------------------------- PUBLIC API: run/stop ---------------------------*/

gboolean
umi_build_runner_run(UmiBuildRunner    *br,    /* runner handle (required)    */
                     char * const      *argv,  /* program + args (NULL-term)  */
                     char * const      *envp,  /* env vector or NULL          */
                     const char        *cwd,   /* working directory or NULL   */
                     UmiBuildExitCb     on_exit,/* optional exit callback     */
                     gpointer           user,  /* user data for callback      */
                     GError           **err)   /* error out (optional)        */
{
  /* Validate the basics: we must have a program (argv[0]).                   */
  if (!br || !argv || !argv[0]) {
    g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                "umi_build_runner_run: invalid runner/argv");
    return FALSE;
  }

  /* Remember the exit callback (if any) and its user pointer.                */
  br->on_exit = on_exit;                                 /* save exit cb      */
  br->user    = user;                                    /* save user data    */

  /* Create a launcher configured to capture stdout/stderr as pipes.          */
  GSubprocessLauncher *launch =
      g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE |
                                G_SUBPROCESS_FLAGS_STDERR_PIPE); /* launcher  */

  /* If a working directory is given, apply it.                               */
  if (cwd && *cwd) {                                     /* non-empty cwd?    */
    g_subprocess_launcher_set_cwd(launch, cwd);          /* set child CWD     */
  }

  /* Environment handling:
   *  - If envp == NULL → inherit parent environment (default; do nothing).
   *  - If envp != NULL → duplicate to a mutable GStrv and hand it to GLib.
   *    NOTE: g_subprocess_launcher_set_environ() “consumes” the vector (GLib
   *    takes ownership), so we MUST NOT free the duplicate ourselves.        */
  if (envp) {                                            /* custom env?       */
    gchar **dup = g_strdupv((gchar * const *)envp);      /* mutable copy      */
    g_subprocess_launcher_set_environ(launch, dup);      /* GLib owns 'dup'   */
    /* no g_strfreev(dup) here — launcher frees it later                     */
  }

  /* Spawn the child process with argv.                                       */
  br->proc = g_subprocess_launcher_spawnv(
      launch,                                            /* launcher          */
      (const gchar * const *)argv,                       /* program + args    */
      err                                                /* error out         */
  );                                                     /* child handle      */

  g_object_unref(launch);                                /* drop launcher     */

  if (!br->proc) {                                       /* spawn failed?     */
    return FALSE;                                        /* *err already set  */
  }

  /* Wrap stdout/stderr pipes with GDataInputStream for line-based async IO.  */
  GInputStream *out_stream = g_subprocess_get_stdout_pipe(br->proc); /* out   */
  GInputStream *err_stream = g_subprocess_get_stderr_pipe(br->proc); /* err   */
  br->out_r = g_data_input_stream_new(out_stream);       /* reader (stdout)   */
  br->err_r = g_data_input_stream_new(err_stream);       /* reader (stderr)   */

  /* Begin reading both streams asynchronously (one line at a time).          */
  read_next_stdout(br);                                  /* arm stdout read   */
  read_next_stderr(br);                                  /* arm stderr read   */

  /* Also wait for the child to exit asynchronously.                          */
  g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br); /* wait  */

  return TRUE;                                           /* launch succeeded  */
}

void
umi_build_runner_stop(UmiBuildRunner *br)
{
  if (!br) return;                                       /* nullptr-safe      */
  g_cancellable_cancel(br->cancel);                      /* cancel any IO     */
  g_subprocess_force_exit(br->proc);                     /* best-effort stop  */
  append_line(br, "[runner] stop requested", TRUE);      /* route a note      */
}

/*--------------------------- INTERNAL HELPERS -------------------------------*/

static void
append_line(UmiBuildRunner *br, const char *s, gboolean is_err)
{
  /* If the caller supplied a sink, route the line; otherwise do nothing.     */
  if (br && br->sink) {                                  /* sink present?     */
    br->sink(br->sink_user, s ? s : "", is_err);         /* deliver text      */
  }
}

static void
read_next_stdout(UmiBuildRunner *br)
{
  if (!br || !br->out_r) return;                         /* guard             */
  g_data_input_stream_read_line_async(
      br->out_r,                                         /* stdout reader     */
      G_PRIORITY_DEFAULT,                                /* priority          */
      br->cancel,                                        /* cancellable       */
      on_read_stdout,                                    /* completion cb     */
      br                                                 /* user data         */
  );
}

static void
read_next_stderr(UmiBuildRunner *br)
{
  if (!br || !br->err_r) return;                         /* guard             */
  g_data_input_stream_read_line_async(
      br->err_r,                                         /* stderr reader     */
      G_PRIORITY_DEFAULT,                                /* priority          */
      br->cancel,                                        /* cancellable       */
      on_read_stderr,                                    /* completion cb     */
      br                                                 /* user data         */
  );
}

static void
on_read_stdout(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;           /* recover runner    */
  gsize len = 0;                                         /* line length out   */
  GError *e = NULL;                                      /* error out         */

  gchar *line = g_data_input_stream_read_line_finish(
      G_DATA_INPUT_STREAM(src),                          /* reader (stdout)   */
      res, &len, &e                                      /* async result      */
  );                                                     /* newly alloc'd str */

  if (e) {                                               /* read error?       */
    append_line(br, e->message, TRUE);                   /* report            */
    g_error_free(e);                                     /* free error        */
    return;                                              /* stop this arm     */
  }

  if (line) {                                            /* got a line        */
    append_line(br, line, FALSE);                        /* route to sink     */
    g_free(line);                                        /* free buffer       */
    read_next_stdout(br);                                /* arm next read     */
  } /* EOF: do nothing — stream is closed */
}

static void
on_read_stderr(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;           /* recover runner    */
  gsize len = 0;                                         /* line length out   */
  GError *e = NULL;                                      /* error out         */

  gchar *line = g_data_input_stream_read_line_finish(
      G_DATA_INPUT_STREAM(src),                          /* reader (stderr)   */
      res, &len, &e                                      /* async result      */
  );                                                     /* newly alloc'd str */

  if (e) {                                               /* read error?       */
    append_line(br, e->message, TRUE);                   /* report            */
    g_error_free(e);                                     /* free error        */
    return;                                              /* stop this arm     */
  }

  if (line) {                                            /* got a line        */
    append_line(br, line, TRUE);                         /* mark as stderr    */
    g_free(line);                                        /* free buffer       */
    read_next_stderr(br);                                /* arm next read     */
  } /* EOF: do nothing */
}

static void
on_proc_exited(GObject *src, GAsyncResult *res, gpointer user)
{
  UmiBuildRunner *br = (UmiBuildRunner *)user;           /* recover runner    */
  GError *e = NULL;                                      /* error out         */

  gboolean ok = g_subprocess_wait_finish(
      G_SUBPROCESS(src),                                 /* child             */
      res, &e                                            /* finish wait       */
  );                                                     /* TRUE if waited    */

  int exit_code = -1;                                    /* default           */
  if (ok) {                                              /* wait succeeded    */
    if (g_subprocess_get_if_exited(G_SUBPROCESS(src)))   /* normal exit?      */
      exit_code = g_subprocess_get_exit_status(G_SUBPROCESS(src)); /* code    */
  } else if (e) {                                        /* wait error        */
    append_line(br, e->message, TRUE);                   /* route error       */
    g_error_free(e);                                     /* drop error        */
  }

  if (br->on_exit) {                                     /* notify caller     */
    br->on_exit(br->user, exit_code);                    /* fire callback     */
  }
}
/*--- end of file ---*/

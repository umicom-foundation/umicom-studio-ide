/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/run_pipeline.c
 *
 * PROJECT:
 *   Umicom Studio IDE — Core “run pipeline” implementation.
 *
 * PURPOSE:
 *   Glue together:
 *     - run_config:      produces argv/envp/cwd for the target to run
 *     - build_runner:    spawns the process, streams stdout/stderr lines
 *     - diagnostics:     routes each line to the problem list + output pane
 *
 * GOALS:
 *   - Pure C, GTK4/GLib/GIO friendly.
 *   - Loosely coupled: depend only on public headers; no cross-folder relative
 *     includes (no "../../...").
 *   - Safe and clear memory ownership: no dangling pointers, no leaks.
 *   - Comprehensive comments (>50%) and line-by-line explanations for clarity.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                   /* GLib core types (gboolean, gpointer, GError)           */
#include "run_pipeline.h"           /* Our own public header (ensures consistent declarations) */

/*--------------------------- Module-private state -------------------------------------------*/
/* We keep a single runner instance and a small “router user data” bundle alive while a child
 * is running. The diagnostics router itself is a lightweight struct of pointers to the UI.   */

typedef struct
{
  UmiDiagRouter router;            /* holds UmiProblemList* + UmiOutputPane*                 */
} UmiRunPipelineCtx;               /* opaque context we pass back to callbacks               */

static UmiBuildRunner   *s_runner  = NULL;   /* singleton runner for the pipeline               */
static UmiRunPipelineCtx *s_ctx    = NULL;   /* lifetime spans start->exit OR start->stop       */

/*--------------------------- Output-line callback -------------------------------------------*/
/* Signature matches the build_runner sink:
 *   typedef void (*UmiOutputSink)(gpointer user, const char *line, gboolean is_err);
 * We forward every line (stdout/stderr) into the diagnostics router, which both mirrors to the
 * Output pane and parses known patterns to populate the Problems list.                         */

static void
on_runner_line(gpointer user, const char *line, gboolean is_err)
{
  /* user  : points to our UmiRunPipelineCtx created at start() time.                         */
  /* line  : a single UTF-8 line without trailing newline (empty string if none).             */
  /* is_err: TRUE if line originated from stderr (can be used to tweak styling if desired).   */

  UmiRunPipelineCtx *ctx = (UmiRunPipelineCtx *)user;        /* recover our context          */
  if (!ctx || !line)                                         /* guard against NULLs          */
    return;                                                  /* nothing to do                */

  /* Feed the text into the diagnostics router. The router will:
   *   - append the line to the Output pane
   *   - parse it for known error/warning formats to update Problems                           */
  umi_diag_router_feed(&ctx->router, line);                  /* route a single line          */

  (void)is_err;  /* currently we treat both streams the same; router/Output pane can style.   */
}

/*--------------------------- Process-exit callback ------------------------------------------*/
/* Signature matches runner exit callback:
 *   typedef void (*UmiBuildExitCb)(gpointer user, int exit_code);
 * We finalize diagnostics (summary/totals), free our context, and keep the runner reusable.    */

static void
on_runner_exit(gpointer user, int exit_code)
{
  UmiRunPipelineCtx *ctx = (UmiRunPipelineCtx *)user;        /* recover our context          */

  if (ctx)
  {
    /* Tell diagnostics we’re done so it can compute a footer/summary if it wants.            */
    umi_diag_router_end(&ctx->router);                       /* finalize problem reporting   */

    g_free(ctx);                                             /* free our small heap context  */
    s_ctx = NULL;                                            /* mark none active             */
  }

  /* Optional: log non-zero exit (could also push to Output pane already via router).         */
  if (exit_code != 0)
  {
    g_warning("run-pipeline: child exited with code=%d", exit_code); /* developer visibility  */
  }
}

/*--------------------------- Public: start the pipeline -------------------------------------*/
/* High-level steps:
 *   1) Ensure we have (or create) a runner instance.
 *   2) Build a diagnostics router anchored on (plist,out); begin() to clear old state.
 *   3) Load run config, obtain argv/envp/cwd using the **actual** API (char** + out_argc).
 *   4) Hook our line/exit callbacks into the runner; spawn the process.
 *   5) Manage ownership: do NOT free argv/envp until after spawn (we free the duplicates).
 *      Keep ctx alive until the exit callback fires (or stop() is called).
 */

gboolean
umi_run_pipeline_start(UmiOutputPane *out, UmiProblemList *plist, GError **err)
{
  /* Validate required UI sinks.                                                               */
  if (!out || !plist)
  {
    g_set_error(err, g_quark_from_static_string("uside-run"), 1,
                "run-pipeline: output pane and problem list must be non-NULL");
    return FALSE;                                            /* cannot proceed               */
  }

  /* Reuse or create the runner.                                                               */
  if (!s_runner)
  {
    s_runner = umi_build_runner_new();                       /* allocate runner              */
    if (!s_runner)
    {
      g_set_error(err, g_quark_from_static_string("uside-run"), 2,
                  "run-pipeline: failed to create runner");
      return FALSE;                                          /* allocation failure           */
    }
  }

  /* Prevent starting a second process while one is active.                                    */
  if (s_ctx)
  {
    g_set_error(err, g_quark_from_static_string("uside-run"), 3,
                "run-pipeline: a process is already running");
    return FALSE;                                            /* enforce single-run invariant */
  }

  /* Create and initialize our tiny context that bridges runner <-> diagnostics router.        */
  s_ctx = (UmiRunPipelineCtx *)g_new0(UmiRunPipelineCtx, 1); /* zero-initialized             */
  s_ctx->router.plist = plist;                               /* connect problem list         */
  s_ctx->router.out   = out;                                 /* connect output console       */

  /* Start a fresh diagnostics session (clears Problems pane, etc.).                           */
  umi_diag_router_begin(&s_ctx->router);                     /* begin routing session        */

  /* Load the run configuration (implementation decides where it comes from: session, file…). */
  UmiRunConfig *rc = umi_run_config_load();                  /* obtain config object         */
  if (!rc)
  {
    g_free(s_ctx); s_ctx = NULL;                             /* drop context on failure      */
    g_set_error(err, g_quark_from_static_string("uside-run"), 4,
                "run-pipeline: failed to load run configuration");
    return FALSE;                                            /* cannot proceed               */
  }

  /* Convert run-config to argv (NULL-terminated) using the real API (char** + out_argc).      */
  int    argc = 0;                                           /* will receive argument count  */
  char **argv = umi_run_config_to_argv(rc, &argc);           /* NULL-terminated vector       */
  if (!argv || !argv[0])                                     /* must have a program to exec  */
  {
    umi_run_config_free(rc);                                 /* free config                  */
    g_free(s_ctx); s_ctx = NULL;                             /* drop context                 */
    g_set_error(err, g_quark_from_static_string("uside-run"), 5,
                "run-pipeline: invalid argv (empty program)");
    return FALSE;                                            /* invalid configuration        */
  }

  /* Convert run-config to envp (NULL-terminated). It may return NULL to mean “inherit”.       */
  char **envp = umi_run_config_to_envp(rc);                  /* may be NULL (inherit env)    */

  /* Working directory for the child; fallback to "." if unspecified.                          */
  const char *cwd = rc->cwd ? rc->cwd : ".";                 /* safe default                 */

  /* Wire our line callback before spawn so early output is not missed.                        */
  umi_build_runner_set_sink(s_runner, on_runner_line, s_ctx);/* sink + user pointer          */

  /* Spawn the process via the runner, passing our **char * const *** vectors as the API wants.
   * (The API uses const on pointed-to strings; GLib historically omits const on gchar**.)     */
  gboolean ok = umi_build_runner_run(
                  s_runner,                                  /* runner instance              */
                  (char * const *)argv,                      /* argv (NULL-terminated)       */
                  (char * const *)envp,                      /* envp (NULL or NULL-terminated)*/
                  cwd,                                       /* working directory            */
                  on_runner_exit,                            /* exit callback                */
                  s_ctx,                                     /* callback user data           */
                  err);                                      /* GLib error out               */

  /* argv/envp were dynamically created by the run-config helpers; free our local copies.      */
  if (argv) g_strfreev(argv);                                /* free vector + strings        */
  if (envp) g_strfreev(envp);                                /* free vector + strings        */

  /* The config object is no longer needed after we’ve produced argv/envp/cwd.                 */
  umi_run_config_free(rc);                                   /* free configuration           */

  if (!ok)
  {
    /* Spawn failed: free context to avoid dangling pointer in callbacks.                      */
    g_free(s_ctx); s_ctx = NULL;                             /* drop context                 */
    return FALSE;                                            /* 'err' already set by runner  */
  }

  /* Success: the child is running; s_ctx remains alive until on_runner_exit() (or stop()).    */
  return TRUE;                                               /* started successfully         */
}

/*--------------------------- Public: stop the pipeline --------------------------------------*/
/* Politely cancels asynchronous reads and requests process termination. We keep the runner
 * instance allocated so a future start() can reuse it. Context will be released by the exit
 * callback after the runner observes process termination.                                      */

void
umi_run_pipeline_stop(void)
{
  if (!s_runner)                                             /* if never created, no-op      */
    return;

  umi_build_runner_stop(s_runner);                            /* request graceful shutdown    */
  /* NOTE:
   *  - on_runner_exit() will be invoked when the child actually terminates.
   *  - on_runner_exit() will free s_ctx, finalize diagnostics, etc.
   */
}
/*--- end of file ---*/

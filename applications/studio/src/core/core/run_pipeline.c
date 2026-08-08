/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/run_pipeline.c
 *
 * PURPOSE:
 *   Orchestrate the “run” action: use run_config to produce argv/envp/cwd,
 *   spawn the target via build_runner, and route all output lines through
 *   diagnostics -> Problems + Output.
 *
 * DESIGN:
 *   - Keeps a small heap context while child is running.
 *   - No deep/relative includes (headers by name only).
 *   - Defensive ownership and error handling with clear comments.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
 
#include <glib.h>
#include "run_pipeline.h"         /* public API we expose                */
#include "run_config.h"           /* umi_run_config_* helpers            */
#include "build_runner.h"         /* umi_build_runner_* APIs             */
#include "diagnostics_router.h"   /* UmiDiagRouter for line routing      */
#include "umi_output_sink.h"      /* umi_output_sink_new for callback    */

/* Small context that wires runner callbacks to our diagnostics router.       */
typedef struct {
  UmiDiagRouter router;           /* holds plist + out + internal parser */
} UmiRunPipelineCtx;

static UmiBuildRunner    *s_runner = NULL;   /* reusable runner instance     */
static UmiRunPipelineCtx *s_ctx    = NULL;   /* alive while child is running */

/* Forward: line callback expected by output sink.                            */
static void on_runner_line(gpointer user, const char *line, gboolean is_err);

/* Start the run pipeline.                                                    */
gboolean
umi_run_pipeline_start(UmiOutputPane *out, UmiProblemList *plist, GError **err)
{
  if (!out || !plist) {
    g_set_error(err, g_quark_from_static_string("uside-run"), 1,
                "output pane and problem list must be non-NULL");
    return FALSE;
  }

  /* Create runner on first use.                                              */
  if (!s_runner) {
    s_runner = umi_build_runner_new();
    if (!s_runner) {
      g_set_error(err, g_quark_from_static_string("uside-run"), 2,
                  "failed to create build runner");
      return FALSE;
    }
  }

  /* Only one child at a time.                                                */
  if (s_ctx) {
    g_set_error(err, g_quark_from_static_string("uside-run"), 3,
                "a process is already running");
    return FALSE;
  }

  /* Build routing context and initialize diagnostics session.                */
  s_ctx = g_new0(UmiRunPipelineCtx, 1);
  s_ctx->router.plist = plist;
  s_ctx->router.out   = out;
  umi_diag_router_begin(&s_ctx->router);

  /* Load run configuration and prepare argv/envp/cwd.                        */
  UmiRunConfig *rc = umi_run_config_load();
  if (!rc) {
    g_free(s_ctx); s_ctx = NULL;
    g_set_error(err, g_quark_from_static_string("uside-run"), 4,
                "failed to load run configuration");
    return FALSE;
  }

  int argc = 0;
  char **argv = umi_run_config_to_argv(rc, &argc);  /* NULL-terminated */
  if (!argv || !argv[0]) {
    umi_run_config_free(rc);
    g_free(s_ctx); s_ctx = NULL;
    g_set_error(err, g_quark_from_static_string("uside-run"), 5,
                "invalid argv from run configuration");
    return FALSE;
  }

  char **envp = umi_run_config_to_envp(rc);        /* may be NULL (inherit)  */
  const char *cwd = rc->cwd ? rc->cwd : ".";

  /* Build a sink that forwards lines to our router.                          */
  UmiOutputSink *sink = umi_output_sink_new(on_runner_line, NULL, s_ctx);
  umi_build_runner_set_sink(s_runner, sink);       /* set concrete sink       */

  /* Split exe and argv-rest for the runner API.                              */
  const char *exe = argv[0];
  const char * const *argv_rest = (const char * const *)(argv + 1);

  /* Spawn and synchronously stream until child exits (runner blocks).        */
  gboolean ok = umi_build_runner_run(
                  s_runner,
                  cwd,
                  exe,
                  argv_rest,
                  (const char * const *)envp,
                  TRUE /* merge stderr to stdout */);

  /* Cleanup transient vectors and config.                                    */
  if (argv) g_strfreev(argv);
  if (envp) g_strfreev(envp);
  umi_run_config_free(rc);

  /* Finalise router and context.                                             */
  umi_diag_router_end(&s_ctx->router);
  g_free(s_ctx); s_ctx = NULL;

  /* NOTE: if umi_output_sink_free() exists in your tree, call it here.       */

  if (!ok) {
    g_set_error(err, g_quark_from_static_string("uside-run"), 6,
                "run target exited with failure");
    return FALSE;
  }
  return TRUE;
}

/* Stop the running process (if any).                                         */
void
umi_run_pipeline_stop(void)
{
  /* Current runner implementation executes synchronously; nothing to stop.   */
  g_warning("umi_run_pipeline_stop: stop not supported by current runner");
}

/* Build runner line callback: route through diagnostics.                     */
static void
on_runner_line(gpointer user, const char *line, gboolean is_err)
{
  (void)is_err;                            /* styling may ignore distinction  */
  UmiRunPipelineCtx *ctx = (UmiRunPipelineCtx *)user;
  if (!ctx || !line) return;
  umi_diag_router_feed(&ctx->router, line);
}
/*  END OF FILE */
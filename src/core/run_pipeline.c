/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/run_pipeline.c
 * PURPOSE: Implementation of the run pipeline
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "run_pipeline.h"
#include "../util/sys/include/env.h"

static UmiBuildRunner *g_runner = NULL;
static UmiDiagRouter   g_router = {0};

static void on_exit(gpointer u, int code){
  (void)u;(void)code;
  umi_diag_router_end(&g_router);
}

gboolean umi_run_pipeline_start(UmiOutputPane *out, UmiProblemList *plist, GError **err){
  UmiRunConfig *rc = umi_run_config_load();
  GPtrArray *argv = umi_run_config_to_argv(rc);
  gchar    **envp = umi_run_config_to_envp(rc);

  g_router.out = out; g_router.plist = plist;
  umi_diag_router_begin(&g_router);

  g_runner = umi_build_runner_new(out);
  gboolean ok = umi_build_runner_run(g_runner, rc->cwd, (char * const*)argv->pdata, on_exit, NULL, err);

  g_ptr_array_free(argv, TRUE);
  g_strfreev(envp);
  umi_run_config_free(rc);
  return ok;
}

void umi_run_pipeline_stop(void){
  if(g_runner){
    umi_build_runner_stop(g_runner);
    umi_build_runner_free(g_runner);
    g_runner = NULL;
  }
}

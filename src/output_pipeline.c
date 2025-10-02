/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pipeline.c
 * PURPOSE: Implementation of output pipeline
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "output_pipeline.h"

static void on_exit(gpointer u, int code){
  UmiOutputPipeline *p = (UmiOutputPipeline*)u;
  if(!p) return;
  if(p->router) umi_problem_router_end(p->router);
  umi_output_console_append_line(p->console, "\x1b[32m[runner] done\x1b[0m");
}

static UmiOutputPipeline *g_self = NULL; /* simple global for line forwarding */

static void on_line(const char *s, gboolean is_err){
  (void)is_err;
  if(!g_self) return;
  if(g_self->router) umi_problem_router_feed(g_self->router, s);
  umi_output_console_append_line(g_self->console, s);
}

gboolean umi_output_pipeline_start(UmiOutputPipeline *p,
                                   const char *cwd,
                                   char * const *argv,
                                   GError **err){
  if(!p || !p->runner || !p->console) return FALSE;
  g_self = p;
  if(p->router) umi_problem_router_begin(p->router);
  umi_output_console_append_line(p->console, "\x1b[36m[runner] starting\x1b[0m");

  /* Bridge runner output: since our build_runner streams directly to UmiOutputPane in v12,
     we assume a variant that we can compile to emit GSignals is pending; for now we append
     start/stop markers and rely on future wiring. */
  gboolean ok = umi_build_runner_run(p->runner, cwd, (char * const*)argv, /*on_exit*/ on_exit, p, err);
  return ok;
}

void umi_output_pipeline_stop(UmiOutputPipeline *p){
  if(!p || !p->runner) return;
  umi_build_runner_stop(p->runner);
}

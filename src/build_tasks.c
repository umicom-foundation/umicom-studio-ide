/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_tasks.c
 * PURPOSE: Implementation of build tasks with verbose comments
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "build_tasks.h"
#include "build_runner.h" /* assumed to exist in your tree */

static gboolean start(UmiBuildTasks *t, GPtrArray* (*fn)(const UmiBuildSys *), GError **err){
  if(!t || !t->bs || !t->console) return FALSE;
  g_autoptr(GPtrArray) argv = fn(t->bs);
  UmiBuildRunner *r = umi_build_runner_new(); /* run in cwd */
  UmiOutputPipeline p = {0}; p.runner = r; p.console = t->console; p.router = NULL;
  gboolean ok = umi_output_pipeline_start(&p, ".", (char * const*)argv->pdata, err);
  return ok;
}

UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputConsole *console){
  UmiBuildTasks *t = g_new0(UmiBuildTasks,1);
  t->bs = umi_buildsys_detect(root?root:".");
  t->console = console;
  return t;
}

gboolean umi_build_tasks_build(UmiBuildTasks *t, GError **err){ return start(t, umi_buildsys_build_argv, err); }
gboolean umi_build_tasks_run  (UmiBuildTasks *t, GError **err){ return start(t, umi_buildsys_run_argv, err); }
gboolean umi_build_tasks_test (UmiBuildTasks *t, GError **err){ return start(t, umi_buildsys_test_argv, err); }

void umi_build_tasks_free(UmiBuildTasks *t){
  if(!t) return;
  umi_buildsys_free(t->bs);
  g_free(t);
}

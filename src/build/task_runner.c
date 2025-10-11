/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/task_runner.c
 * PURPOSE: Implementation of background task runner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "task_runner.h"

static void adapter(gpointer data, gpointer user_data){
  (void)user_data;
  UmiTaskFn fn = (UmiTaskFn)data;
  if(fn) fn(NULL);
}

UmiTaskRunner *umi_task_runner_new(int max_threads){
  UmiTaskRunner *r = g_new0(UmiTaskRunner,1);
  GError *e=NULL;
  r->pool = g_thread_pool_new(adapter, NULL, max_threads>0?max_threads:2, FALSE, &e);
  if(e){ g_warning("thread pool: %s", e->message); g_error_free(e); }
  return r;
}

void umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user){
  (void)user;
  if(!r || !r->pool || !fn) return;
  g_thread_pool_push(r->pool, (gpointer)fn, NULL);
}

void umi_task_runner_free(UmiTaskRunner *r){
  if(!r) return;
  if(r->pool) g_thread_pool_free(r->pool, FALSE, TRUE);
  g_free(r);
}

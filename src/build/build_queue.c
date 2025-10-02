/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_queue.c
 * PURPOSE: Implementation of a simple FIFO queue for build/run jobs
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "build_queue.h"

typedef struct {
  gchar *cwd;
  GPtrArray *argv; /* char* array, NULL-terminated when used */
} Job;

struct _UmiBuildQueue {
  UmiOutputPane *out;
  UmiBuildRunner *runner;
  GQueue *jobs; /* Job* */
  gboolean running;
};

static Job *job_new(const char *cwd, char * const *argv){
  Job *j = g_new0(Job,1);
  j->cwd = g_strdup(cwd?cwd:".");
  j->argv = g_ptr_array_new_with_free_func(g_free);
  if(argv){
    for(char * const *p=argv; *p; ++p) g_ptr_array_add(j->argv, g_strdup(*p));
  }else{
    g_ptr_array_add(j->argv, g_strdup("true"));
  }
  g_ptr_array_add(j->argv, NULL);
  return j;
}

static void job_free(Job *j){
  if(!j) return;
  g_free(j->cwd);
  g_ptr_array_free(j->argv, TRUE);
  g_free(j);
}

UmiBuildQueue *umi_build_queue_new(UmiOutputPane *out){
  UmiBuildQueue *q = g_new0(UmiBuildQueue,1);
  q->out = out;
  q->jobs = g_queue_new();
  return q;
}

void umi_build_queue_push(UmiBuildQueue *q, const char *cwd, char * const *argv){
  if(!q) return;
  g_queue_push_tail(q->jobs, job_new(cwd, argv));
  umi_output_pane_append_line(q->out, "[queue] job enqueued");
}

static void on_done(gpointer user, int code){
  UmiBuildQueue *q = (UmiBuildQueue*)user;
  umi_output_pane_append_line(q->out, "[queue] job finished");
  q->running = FALSE;
  q->runner = NULL;
  /* Start next */
  if(!g_queue_is_empty(q->jobs)) umi_build_queue_start(q);
}

gboolean umi_build_queue_start(UmiBuildQueue *q){
  if(!q || q->running || g_queue_is_empty(q->jobs)) return FALSE;
  Job *j = (Job*)g_queue_pop_head(q->jobs);
  q->runner = umi_build_runner_new(q->out);
  GError *e=NULL;
  gboolean ok = umi_build_runner_run(q->runner, j->cwd, (char * const*)j->argv->pdata, on_done, q, &e);
  if(!ok && e){ umi_output_pane_append_line_err(q->out, e->message); g_error_free(e); }
  q->running = ok;
  job_free(j);
  return ok;
}

void umi_build_queue_stop(UmiBuildQueue *q){
  if(!q) return;
  if(q->runner) umi_build_runner_stop(q->runner);
  while(!g_queue_is_empty(q->jobs)) job_free((Job*)g_queue_pop_head(q->jobs));
}

void umi_build_queue_free(UmiBuildQueue *q){
  if(!q) return;
  umi_build_queue_stop(q);
  if(q->runner) umi_build_runner_free(q->runner);
  g_queue_free(q->jobs);
  g_free(q);
}

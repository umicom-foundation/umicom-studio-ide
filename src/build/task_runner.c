/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/task_runner.c
 * PURPOSE:
 *   Implementation of a tiny, GLib-based task runner. Provides a small API
 *   to queue background jobs (function + user_data) onto a GThreadPool, so
 *   the GUI remains responsive while work runs in parallel.
 *
 * HOW IT WORKS (PSEUDOCODE):
 *   struct Runner { GThreadPool *pool; }
 *   new(max_threads):
 *     threads = clamp(max_threads, 1..N)
 *     pool = g_thread_pool_new(worker_invoke, /*user_data=*/NULL, threads, exclusive=FALSE)
 *   queue(fn, user):
 *     job = allocate { fn, user }
 *     g_thread_pool_push(pool, job)
 *   worker_invoke(job):
 *     call job->fn(job->user)
 *     free(job)
 *   free():
 *     g_thread_pool_free(pool, immediate=FALSE, wait_=TRUE)
 *
 * CONCURRENCY RULE:
 *   Worker threads MUST NOT touch GTK. UI updates must be posted back to
 *   the main thread (e.g., via g_idle_add()).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "task_runner.h"   /* <- public API for this module only            */
#include <stdlib.h>        /* <- for NULL                                   */

/*----------------------------- Internal Types ------------------------------*/
/* A single queued call: function pointer + user data.                       */
/* Keeping this tiny helps throughput and reduces allocation overhead.       */
typedef struct UmiTaskCall {
  UmiTaskFn fn;           /* function to invoke on a worker thread          */
  gpointer  user;         /* opaque context pointer passed to 'fn'          */
} UmiTaskCall;

/* The opaque runner holds only a GLib thread pool.                          */
struct UmiTaskRunner {
  GThreadPool *pool;      /* shared pool executing our UmiTaskCall jobs     */
};

/*----------------------------- Worker Glue ---------------------------------*/
/* worker_invoke:
 *  Signature must match GFunc: void (*GFunc)(gpointer data, gpointer user_data)
 *  - 'data'      : our UmiTaskCall*
 *  - 'user_data' : constant pointer supplied once at pool creation (unused)
 */
static void
worker_invoke(gpointer data, gpointer user_data /* unused */)
{
  /* Cast the generic pointer back to our job container.                     */
  UmiTaskCall *call = (UmiTaskCall *)data;        /* recover the enqueued job */

  /* Defensive: ensure we have a function before calling it.                 */
  if (call && call->fn) {                         /* if valid job            */
    /* CRITICAL: pass the user pointer to match typedef void (*)(gpointer).  */
    call->fn(call->user);                         /* run job with its arg    */
  }

  /* Free the small job object. Using GLib slice is fast & low-fragmentation */
  g_slice_free(UmiTaskCall, call);                /* return memory to slice  */
}

/*----------------------------- API: new/free -------------------------------*/
UmiTaskRunner *
umi_task_runner_new(int max_threads)
{
  /* Clamp the requested size: GLib expects positive values for pools.       */
  int threads = (max_threads > 0) ? max_threads : 1;  /* at least one worker */

  /* Allocate our runner object from GLib's slice allocator.                 */
  UmiTaskRunner *r = g_slice_new0(UmiTaskRunner);     /* zeroed memory       */
  if (!r) {                                          /* allocation failure?  */
    return NULL;                                     /* bubble up NULL       */
  }

  /* Create a non-exclusive thread pool that will execute 'worker_invoke'.   */
  /* - user_data   : NULL (we don't need a constant context)                 */
  /* - exclusive   : FALSE (allow GLib to share the global pool infra)       */
  /* - error out   : for now, if creation fails, free 'r' and return NULL    */
  GError *err = NULL;                                 /* collect GLib errors */
  r->pool = g_thread_pool_new(/*func=*/worker_invoke,
                              /*user_data=*/NULL,
                              /*max_threads=*/threads,
                              /*exclusive=*/FALSE,
                              /*error=*/&err);
  if (!r->pool) {                                     /* pool could not be made */
    g_slice_free(UmiTaskRunner, r);                   /* free runner object     */
    if (err) g_error_free(err);                       /* drop error (optional)  */
    return NULL;                                      /* signal failure         */
  }

  return r;                                           /* ready to queue work   */
}

void
umi_task_runner_free(UmiTaskRunner *r)
{
  if (!r) return;                                     /* nullptr-safe         */

  /* Free the thread pool:                                                   
   *  - immediate = FALSE (do not free pool if jobs still queued)
   *  - wait_     = TRUE  (block until running jobs finish)
   * This guarantees no background thread touches freed memory after return.
   */
  g_thread_pool_free(r->pool, /*immediate=*/FALSE, /*wait_=*/TRUE);

  /* Free the runner object itself.                                          */
  g_slice_free(UmiTaskRunner, r);
}

/*----------------------------- API: queue ----------------------------------*/
void
umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user)
{
  if (!r || !fn) return;                              /* invalid inputs?      */

  /* Allocate a tiny job package to carry the pointer + its user data.       */
  UmiTaskCall *call = g_slice_new(UmiTaskCall);       /* small, fast alloc    */
  call->fn   = fn;                                    /* store function       */
  call->user = user;                                  /* store user context   */

  /* Push onto the GLib thread pool. If the pool is full, GLib will queue.   */
  /* We ignore the (rare) push error here; production code could log it.     */
  GError *err = NULL;                                 /* collect push errors  */
  g_thread_pool_push(r->pool, call, &err);            /* enqueue the job      */
  if (G_UNLIKELY(err)) {                              /* if push failed       */
    g_slice_free(UmiTaskCall, call);                  /* free job we created  */
    g_error_free(err);                                /* drop error object    */
    /* Optionally log via your logger here.                                   */
  }
}

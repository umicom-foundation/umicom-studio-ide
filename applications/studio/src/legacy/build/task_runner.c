/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/task_runner.c
 * PURPOSE:
 *   Implementation of a tiny, GLib-based task runner. The runner owns a
 *   GThreadPool and lets callers queue background jobs (function + user_data)
 *   so the GTK main loop stays responsive while work runs off-thread.
 *
 * DESIGN / ARCH:
 *   - Pure C, no C++; depends only on GLib threading primitives.
 *   - Loosely coupled: only includes this module’s own public header.
 *   - Opaque handle: callers can’t access internals (reduces coupling).
 *   - Each job is a pair {UmiTaskFn fn, gpointer user}; workers call fn(user).
 *   - DO NOT touch GTK from workers; post back with g_idle_add() if needed.
 *
 * SAFETY:
 *   - Input validation on all public APIs.
 *   - Defensive checks before invoking callbacks.
 *   - Errors during push are logged and the job is freed (no leaks).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "task_runner.h"                 /* module’s public API; brings in GLib */
#include <stdlib.h>                      /* NULL macro                          */

/*--------------------------- Internal Structures ----------------------------*/

/* A single enqueued call (function + user data).                              */
typedef struct UmiTaskCall {
  UmiTaskFn fn;                          /* the function to run on a worker    */
  gpointer  user;                        /* opaque pointer passed to the fn    */
} UmiTaskCall;

/* Opaque runner body: keep internals private to this .c file.                 */
struct UmiTaskRunner {
  GThreadPool *pool;                     /* GLib thread pool for executing jobs*/
};

/*--------------------------- Forward Declarations ---------------------------*/

/* Worker trampoline that matches GFunc: void (*)(gpointer,gpointer).          */
static void worker_invoke(gpointer data, gpointer user_data);

/*------------------------------ API: Create --------------------------------*/

UmiTaskRunner *
umi_task_runner_new(int max_threads)     /* create a runner with N workers     */
{
  int threads = (max_threads > 0) ? max_threads : 1; /* clamp to >= 1         */
  GError *err = NULL;                    /* will capture GLib creation errors  */

  /* Create a shared (non-exclusive) thread pool.                              */
  GThreadPool *pool = g_thread_pool_new(
      worker_invoke,                     /* function applied to each job       */
      NULL,                              /* pool user_data (unused by us)      */
      threads,                           /* maximum threads allowed            */
      FALSE,                             /* shared, not exclusive              */
      &err                               /* error out                          */
  );

  if (!pool) {                           /* creation failed?                   */
    if (err) {                           /* log diagnostic if provided         */
      g_warning("umi_task_runner_new: %s", err->message ? err->message : "unknown");
      g_clear_error(&err);               /* free error                         */
    }
    return NULL;                         /* signal failure to caller           */
  }

  /* Allocate the runner and store the pool.                                   */
  UmiTaskRunner *r = g_slice_new0(UmiTaskRunner); /* zero-initialized struct  */
  r->pool = pool;                        /* keep the created pool              */
  return r;                              /* hand back to caller                */
}

/*------------------------------ API: Destroy -------------------------------*/

void
umi_task_runner_free(UmiTaskRunner *r)   /* destroy the runner + pool          */
{
  if (!r) return;                        /* guard against NULL                 */

  if (r->pool) {                         /* if we have a pool                  */
    /* Free the pool:
     *  immediate = FALSE  => do not kill running tasks abruptly
     *  wait_     = TRUE   => block until queued/running tasks complete
     */
    g_thread_pool_free(r->pool, FALSE, TRUE); /* clean shutdown               */
    r->pool = NULL;                      /* clear dangling pointer             */
  }

  g_slice_free(UmiTaskRunner, r);        /* free the runner struct itself      */
}

/*------------------------------ API: Queue ---------------------------------*/

void
umi_task_runner_queue(UmiTaskRunner *r,  /* enqueue a job onto the pool        */
                      UmiTaskFn fn,      /* callback function (runs on worker) */
                      gpointer user)     /* opaque user_data passed to fn      */
{
  if (!r || !r->pool || !fn) {           /* validate inputs                    */
    return;                              /* nothing to do if invalid           */
  }

  /* Allocate a small job container.                                          */
  UmiTaskCall *job = g_slice_new(UmiTaskCall); /* allocate from GLib slice    */
  job->fn   = fn;                        /* store function pointer             */
  job->user = user;                      /* store opaque context               */

  /* Push the job to the thread pool.                                         */
  GError *err = NULL;                    /* capture potential push error       */
  g_thread_pool_push(r->pool, job, &err);/* async enqueue                      */
  if (G_UNLIKELY(err != NULL)) {         /* if push failed                     */
    g_warning("umi_task_runner_queue: %s", err->message ? err->message : "unknown");
    g_clear_error(&err);                 /* free GLib error                    */
    g_slice_free(UmiTaskCall, job);      /* free the job to avoid leaking      */
  }
}

/*---------------------------- Worker Trampoline ----------------------------*/

/* worker_invoke:
 *  GLib calls this for each queued item. Its signature must be:
 *    void (*GFunc)(gpointer data, gpointer user_data)
 *  - data      : the UmiTaskCall* we pushed
 *  - user_data : the constant pointer given to g_thread_pool_new (unused here)
 */
static void
worker_invoke(gpointer data, gpointer user_data)
{
  (void)user_data;                       /* we don’t use pool user_data        */

  UmiTaskCall *call = (UmiTaskCall *)data; /* recover the job object          */
  if (call && call->fn) {                /* defensive: ensure a valid fn       */
    call->fn(call->user);                /* run the job: fn(user)              */
  }

  g_slice_free(UmiTaskCall, call);       /* always free the job container      */
}

/*--------------------------------- EOF -------------------------------------*/

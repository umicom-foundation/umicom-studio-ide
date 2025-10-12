/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/task_runner.h
 * PURPOSE:
 *   Minimal, thread-pool-based task runner for running small background jobs
 *   without blocking the GTK main loop. The runner owns a GLib GThreadPool
 *   and lets callers queue simple "function + user_data" jobs.
 *
 * DESIGN CHOICES:
 *   - Pure C + GLib only. No C++.
 *   - Loosely-coupled: header is self-contained; no cross-module includes.
 *   - Opaque type: callers cannot reach into internals (reduces coupling).
 *   - Safe handoff: each queued job receives its user_data pointer as the
 *     single argument, matching the typedef below.
 *
 * REQUIREMENTS:
 *   - GLib threading (GThreadPool) is available and initialized implicitly
 *     by GLib when used. GUI code must queue work, never touch GTK from
 *     worker threads.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TASK_RUNNER_H
#define UMICOM_TASK_RUNNER_H

/* Include GLib for thread pool, basic types, and gpointer.                  */
#include <glib.h>     /* <- provides GThreadPool, gpointer, GError, etc.    */

/* Forward declaration of the opaque runner type.                            */
/* We hide its definition in the .c file to keep modules loosely-coupled.    */
typedef struct UmiTaskRunner UmiTaskRunner;

/* Define the function type a task must implement.                           */
/* Each job is: void (*fn)(gpointer user_data)                               */
/* This allows passing any context pointer without capturing global state.   */
typedef void (*UmiTaskFn)(gpointer user_data);

/* Create a runner with up to 'max_threads' worker threads.                  */
/* If max_threads <= 0, we clamp to 1 to avoid undefined GLib behavior.      */
/* Returns a heap-allocated runner, or NULL on allocation error.             */
UmiTaskRunner *umi_task_runner_new(int max_threads);

/* Destroy the runner, freeing underlying resources.                         */
/* Any queued but not-yet-started jobs are dropped when freeing the pool.    */
void           umi_task_runner_free(UmiTaskRunner *r);

/* Queue a job for asynchronous execution.                                   */
/* 'fn'   : function pointer to call on a worker thread.                     */
/* 'user' : opaque pointer passed as the sole argument to 'fn'.              */
/* Safe to call from the GTK main thread. Returns immediately.               */
void           umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user);

#endif /* UMICOM_TASK_RUNNER_H */

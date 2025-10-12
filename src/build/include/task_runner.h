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
 *   - Opaque type: callers cannot reach internals (reduces coupling).
 *   - Safe handoff: each queued job receives its user_data pointer as the
 *     single argument, matching the typedef below.
 *
 * REQUIREMENTS:
 *   - GLib threading (GThreadPool) available and implicitly initialized by GLib.
 *   - Never touch GTK from worker threads (post back via g_idle_add()).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TASK_RUNNER_H
#define UMICOM_TASK_RUNNER_H

/* Bring in GLib basic types and GThreadPool.                                   */
#include <glib.h>  /* gboolean, gpointer, GThreadPool, GError, etc. */

/* Opaque runner (internals hidden in .c for loose coupling).                   */
typedef struct UmiTaskRunner UmiTaskRunner;

/* Function type for a queued job: void job(gpointer user_data).                */
typedef void (*UmiTaskFn)(gpointer user_data);

/* Create a runner with up to 'max_threads' workers (clamped to >=1).           */
UmiTaskRunner *umi_task_runner_new(int max_threads);

/* Destroy the runner and its thread pool.                                      */
void           umi_task_runner_free(UmiTaskRunner *r);

/* Queue a job for async execution on the pool.                                 */
void           umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user);

#endif /* UMICOM_TASK_RUNNER_H */
/*---------------------------------------------------------------------------*/
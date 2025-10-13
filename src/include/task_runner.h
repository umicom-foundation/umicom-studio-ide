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

#include <glib.h>

G_BEGIN_DECLS

typedef struct UmiTaskRunner UmiTaskRunner;

typedef void (*UmiTaskFn)(gpointer user_data);

/* Create a runner with up to 'max_threads' worker threads (clamped to >= 1). */
UmiTaskRunner *umi_task_runner_new(int max_threads);

/* Destroy the runner and its thread pool. */
void           umi_task_runner_free(UmiTaskRunner *r);

/* Queue a job for asynchronous execution on the pool. */
void           umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user);

G_END_DECLS
#endif /* UMICOM_TASK_RUNNER_H */

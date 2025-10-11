/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/task_runner.h
 * PURPOSE: Tiny background task queue using GThreadPool
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TASK_RUNNER_H
#define UMICOM_TASK_RUNNER_H

#include <glib.h>

typedef void (*UmiTaskFn)(gpointer user);

typedef struct {
  GThreadPool *pool; /* executes tasks in the background */
} UmiTaskRunner;

UmiTaskRunner *umi_task_runner_new(int max_threads);
void           umi_task_runner_queue(UmiTaskRunner *r, UmiTaskFn fn, gpointer user);
void           umi_task_runner_free(UmiTaskRunner *r);

#endif /* UMICOM_TASK_RUNNER_H */

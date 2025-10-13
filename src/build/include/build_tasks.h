/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_tasks.h
 * PURPOSE: Tiny build task facade that uses build_system + output pipeline
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_TASKS_H
#define UMICOM_BUILD_TASKS_H

#include "build_system.h"

#include "umi_output_sink.h"


typedef struct {
  UmiBuildSys *bs;
  UmiOutputConsole *console;
} UmiBuildTasks;

/* Create tasks by detecting build tool in 'root' and binding a console. */
UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputConsole *console);

/* Start build/run/test; returns FALSE if we cannot spawn. */
gboolean umi_build_tasks_build(UmiBuildTasks *t, GError **err);
gboolean umi_build_tasks_run  (UmiBuildTasks *t, GError **err);
gboolean umi_build_tasks_test (UmiBuildTasks *t, GError **err);

/* Free resources. */
void umi_build_tasks_free(UmiBuildTasks *t);

#endif /* UMICOM_BUILD_TASKS_H */

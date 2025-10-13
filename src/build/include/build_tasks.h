/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_tasks.h
 *
 * PURPOSE:
 *   High-level "Build Tasks" façade that coordinates:
 *     - selecting the active build system (ninja/make/msbuild/custom),
 *     - invoking the Build Runner (child process w/ streamed output),
 *     - routing output lines to a UI-agnostic UmiOutputSink,
 *     - simple helpers to Build / Run / Test a project rooted at 'root'.
 *
 * DECOUPLING:
 *   Avoid direct includes of UI console types. Instead depend only on the tiny
 *   callback contract declared in src/include/umi_output_sink.h.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_TASKS_H
#define UMICOM_BUILD_TASKS_H

#include <glib.h>
#include "umi_output_sink.h"
#include "build_runner.h"
#include "build_system.h"

typedef struct _UmiBuildTasks UmiBuildTasks;

/* Construct / destroy. The sink may be NULL (output is dropped). */
UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputSink sink, gpointer sink_user);
void           umi_build_tasks_free(UmiBuildTasks *bt);

/* Change output routing after construction. */
void           umi_build_tasks_set_sink(UmiBuildTasks *bt, UmiOutputSink sink, gpointer sink_user);

/* Actions. Return TRUE if process launch succeeded (not tool success). */
gboolean       umi_build_tasks_build(UmiBuildTasks *bt, GString *err);
gboolean       umi_build_tasks_run  (UmiBuildTasks *bt, GString *err);
gboolean       umi_build_tasks_test (UmiBuildTasks *bt, GString *err);

/* Read-only information. */
const char    *umi_build_tasks_root(const UmiBuildTasks *bt);

#endif /* UMICOM_BUILD_TASKS_H */
/*---------------------------------------------------------------------------*/

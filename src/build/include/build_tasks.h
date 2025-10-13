/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/build/include/build_tasks.h
 *
 * PURPOSE:
 *   High-level operations for a project root (build / run / test).
 *   Wraps the lower-level UmiBuildRunner and a detected UmiBuildSys.
 *
 * DESIGN:
 *   - No UI headers here; caller provides an UmiOutputSink instance.
 *   - Opaque UmiBuildTasks handle; implementation is private to .c
 *
 * API:
 *   UmiBuildTasks *umi_build_tasks_new   (const char *root, UmiOutputSink *sink);
 *   void           umi_build_tasks_free  (UmiBuildTasks *t);
 *   void           umi_build_tasks_set_sink(UmiBuildTasks *t, UmiOutputSink *sink);
 *   gboolean       umi_build_tasks_build (UmiBuildTasks *t, GError **error);
 *   gboolean       umi_build_tasks_run   (UmiBuildTasks *t, GError **error);
 *   gboolean       umi_build_tasks_test  (UmiBuildTasks *t, GError **error);
 *   const char    *umi_build_tasks_root  (const UmiBuildTasks *t);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_BUILD_TASKS_H
#define UMI_BUILD_TASKS_H

#include <glib.h>
#include <umi_output_sink.h>  /* decoupled sink */

G_BEGIN_DECLS

typedef struct _UmiBuildTasks UmiBuildTasks; /* opaque */

UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputSink *sink);
void           umi_build_tasks_free(UmiBuildTasks *t);

void           umi_build_tasks_set_sink(UmiBuildTasks *t, UmiOutputSink *sink);

gboolean       umi_build_tasks_build(UmiBuildTasks *t, GError **error);
gboolean       umi_build_tasks_run  (UmiBuildTasks *t, GError **error);
gboolean       umi_build_tasks_test (UmiBuildTasks *t, GError **error);

const char    *umi_build_tasks_root (const UmiBuildTasks *t);

G_END_DECLS

#endif /* UMI_BUILD_TASKS_H */

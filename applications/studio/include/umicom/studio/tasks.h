/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/tasks.h
 *
 * PURPOSE:
 *   Define Studio task submission and completion helpers over the Framework
 *   task queue so build, search, indexing and language work share one worker
 *   infrastructure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_TASKS_H
#define UMICOM_STUDIO_TASKS_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_tasks_submit(UmiStudioServices *services,
                                  const UmiTaskConfig *config,
                                  UmiTask **out_task);
UmiStatus umi_studio_tasks_wait_and_destroy(UmiTask *task,
                                            uint32_t timeout_ms);
UmiTaskQueueStats umi_studio_tasks_stats(UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif

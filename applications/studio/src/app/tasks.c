/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/tasks.c
 *
 * PURPOSE:
 *   Implement Studio task creation, queue submission, completion waiting and
 *   ownership cleanup through the shared Framework task service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/tasks.h"

UmiStatus umi_studio_tasks_submit(UmiStudioServices *services,
                                  const UmiTaskConfig *config,
                                  UmiTask **out_task)
{
    UmiTaskQueue *queue = umi_studio_services_task_queue(services);
    UmiTask *task = NULL;
    UmiStatus status;

    if (queue == NULL || config == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_task = NULL;

    status = umi_task_create(config, &task);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_task_queue_submit(queue, task);
    if (status != UMI_STATUS_OK) {
        umi_task_destroy(task);
        return status;
    }

    *out_task = task;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_tasks_wait_and_destroy(UmiTask *task,
                                            uint32_t timeout_ms)
{
    UmiStatus status;
    UmiStatus result;

    if (task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_task_wait(task, timeout_ms);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    result = umi_task_result(task);
    umi_task_destroy(task);
    return result;
}

UmiTaskQueueStats umi_studio_tasks_stats(UmiStudioServices *services)
{
    return umi_task_queue_stats(umi_studio_services_task_queue(services));
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_tasks.c
 *
 * PURPOSE:
 *   Verify that Studio submits background work through the shared Framework
 *   task queue and retains clear task ownership and completion semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/tasks.h"

#include <assert.h>
#include <stdatomic.h>

static UmiStatus work(UmiTaskContext *context, void *user_data)
{
    atomic_int *counter = (atomic_int *)user_data;
    (void)atomic_fetch_add(counter, 1);
    return umi_task_context_report(context, 100U, "done");
}

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiTask *task = NULL;
    UmiTaskConfig config;
    atomic_int counter;
    UmiTaskQueueStats stats;

    (void)umi_fs_remove_tree(".umicom");
    atomic_init(&counter, 0);
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);

    config.label = "Studio task";
    config.function = work;
    config.user_data = &counter;
    config.progress_sink = NULL;
    config.progress_user_data = NULL;

    assert(umi_studio_tasks_submit(services,
                                   &config,
                                   &task) == UMI_STATUS_OK);
    assert(umi_studio_tasks_wait_and_destroy(task,
                                             5000U) == UMI_STATUS_OK);
    assert(atomic_load(&counter) == 1);

    stats = umi_studio_tasks_stats(services);
    assert(stats.submitted == 1U);
    assert(stats.completed == 1U);
    assert(stats.failed == 0U);

    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}

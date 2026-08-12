/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_task_centre.c
 *
 * PURPOSE:
 *   Verify Studio task browsing and submission share the Framework project runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_workbench.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperTaskCentre *centre;
    UmiStudioDeveloperTaskCentreSnapshot snapshot;
    UmiProjectTaskSnapshot task = {0};
    UmiDeveloperTaskPlanRequest request = {0};
    UmiDeveloperTaskPlanSnapshot plan;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_task_centre(workbench);
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.id, "studio.verify"); strcpy(task.project_id, "studio");
    strcpy(task.label, "Verify Studio"); strcpy(task.command, "ctest --test-dir build");
    task.enabled = 1;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(
            umi_studio_developer_task_centre_runtime(centre))), &task)
        == UMI_STATUS_OK);
    assert(umi_studio_developer_task_centre_snapshot(centre, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.task_count == 1U);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_TASK_PLAN_API_VERSION;
    request.task_id = "studio.verify";
    assert(umi_studio_developer_task_centre_submit(centre, &request, &plan) == UMI_STATUS_OK);
    assert(strcmp(plan.task_id, "studio.verify") == 0);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

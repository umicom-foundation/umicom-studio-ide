/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_workflow_centre.c
 *
 * PURPOSE:
 *   Verify Studio exposes task/launch workflow composition through its pipeline centre.
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
    UmiStudioDeveloperPipelineCentre *centre;
    UmiDeveloperRuntime *runtime;
    UmiProjectTaskSnapshot task = {0};
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiDeveloperWorkflowRequest request = {0};
    UmiDeveloperWorkflowSnapshot workflow;
    const char *tasks[] = {"build"};

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_pipeline(workbench);
    runtime = umi_studio_developer_pipeline_centre_runtime(centre);

    task.struct_size=(uint32_t)sizeof(task); task.api_version=UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.id,"build"); strcpy(task.project_id,"studio"); strcpy(task.label,"Build");
    strcpy(task.command,"cmake --build build"); task.enabled=1;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(umi_developer_runtime_projects(runtime)), &task)==UMI_STATUS_OK);
    profile.struct_size=(uint32_t)sizeof(profile); profile.api_version=UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id,"run"); strcpy(profile.project_id,"studio"); strcpy(profile.name,"Run");
    strcpy(profile.program,"umicom-studio-console");
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(runtime)), &profile)==UMI_STATUS_OK);

    request.struct_size=(uint32_t)sizeof(request); request.api_version=UMI_DEVELOPER_WORKFLOW_API_VERSION;
    request.workflow_id="verify"; request.task_ids=tasks; request.task_count=1U;
    request.launch_profile_id="run"; request.debug_mode=0;
    assert(umi_studio_developer_pipeline_centre_submit_workflow(centre,&request,&workflow)==UMI_STATUS_OK);
    assert(workflow.operation_count==2U); assert(workflow.dependency_count==1U);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

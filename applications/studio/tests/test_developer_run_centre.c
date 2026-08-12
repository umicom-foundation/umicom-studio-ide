/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_run_centre.c
 *
 * PURPOSE:
 *   Verify Studio run/debug profiles use the shared Framework runtime.
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
    UmiStudioDeveloperRunCentre *centre;
    UmiStudioDeveloperRunCentreSnapshot snapshot;
    UmiProjectLaunchProfileSnapshot profile = {0};
    UmiDeveloperLaunchPlanRequest request = {0};
    UmiDeveloperLaunchPlanSnapshot plan;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_run_centre(workbench);
    profile.struct_size = (uint32_t)sizeof(profile);
    profile.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(profile.id, "studio.console"); strcpy(profile.project_id, "studio");
    strcpy(profile.name, "Studio Console"); strcpy(profile.program, "umicom-studio-console");
    profile.debug = 1;
    assert(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(umi_developer_runtime_projects(
            umi_studio_developer_run_centre_runtime(centre))), &profile)
        == UMI_STATUS_OK);
    assert(umi_studio_developer_run_centre_snapshot(centre, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.launch_profile_count == 1U);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
    request.launch_profile_id = "studio.console";
    request.debug_mode = -1;
    assert(umi_studio_developer_run_centre_submit(centre, &request, &plan) == UMI_STATUS_OK);
    assert(plan.operation_kind == UMI_DEVELOPER_OPERATION_DEBUG);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_workspace_state_centre.c
 *
 * PURPOSE:
 *   Verify Studio can persist and restore the shared developer context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_workbench.h"
#include "umicom/platform/filesystem.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperWorkspaceStateCentre *centre;
    UmiStudioDeveloperWorkspaceStateCentreSnapshot snapshot;
    UmiDeveloperContextSnapshot context = {0};
    char temp[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_workspace_state(workbench);
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    strcpy(context.project_id, "studio");
    strcpy(context.workspace_directory, "C:\\Dev\\umicom\\umicom-studio");
    assert(umi_developer_runtime_set_context(
        umi_studio_developer_workspace_state_centre_runtime(centre), &context)
        == UMI_STATUS_OK);
    assert(umi_studio_developer_workspace_state_centre_set_preferences(
        centre, "build.all", "studio.console", 1, 1, 1) == UMI_STATUS_OK);
    assert(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), temp, "umicom-studio-state-centre-test.txt") == UMI_STATUS_OK);
    assert(umi_studio_developer_workspace_state_centre_save(centre, path) == UMI_STATUS_OK);

    memset(&context, 0, sizeof(context));
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
    strcpy(context.project_id, "changed");
    assert(umi_developer_runtime_set_context(
        umi_studio_developer_workspace_state_centre_runtime(centre), &context)
        == UMI_STATUS_OK);
    assert(umi_studio_developer_workspace_state_centre_load(centre, path) == UMI_STATUS_OK);
    assert(umi_studio_developer_workspace_state_centre_snapshot(centre, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.state.context.project_id, "studio") == 0);
    assert(strcmp(snapshot.state.last_task_id, "build.all") == 0);
    (void)remove(path);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

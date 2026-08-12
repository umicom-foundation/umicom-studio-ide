/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/developer_state_main.c
 *
 * PURPOSE:
 *   Provide a native command for inspecting and updating the persistent
 *   developer workspace state used by Studio and future development tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * With only a state-file path this command loads and prints the stored context.
 * Supplying a project id and workspace directory updates the shared Framework
 * developer context first and then writes the state through atomic replacement.
 */
#include "umicom/studio/developer_workbench.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

static void print_state(const UmiStudioDeveloperWorkspaceStateCentreSnapshot *snapshot)
{
    printf("Developer workspace state\n");
    printf("  project: %s\n", snapshot->state.context.project_id);
    printf("  configuration: %s\n", snapshot->state.context.configuration_id);
    printf("  target: %s\n", snapshot->state.context.target_id);
    printf("  launch profile: %s\n", snapshot->state.context.launch_profile_id);
    printf("  workspace: %s\n", snapshot->state.context.workspace_directory);
    printf("  active document: %s\n", snapshot->state.context.active_document);
    printf("  last task: %s\n", snapshot->state.last_task_id);
    printf("  last launch: %s\n", snapshot->state.last_launch_profile_id);
    printf("  revision: %llu\n", (unsigned long long)snapshot->state.revision);
}

int main(int argc, char **argv)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperWorkspaceStateCentre *centre;
    UmiStudioDeveloperWorkspaceStateCentreSnapshot snapshot;
    UmiDeveloperContextSnapshot context;
    UmiStatus status;

    if (argc < 2 || argc > 4) {
        fprintf(stderr,
                "Usage: %s <state-file> [project-id] [workspace-directory]\n",
                argv[0]);
        return 2;
    }

    status = umi_studio_developer_workbench_create(&workbench);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Failed to create developer workbench: %s\n",
                umi_status_text(status));
        return 1;
    }
    centre = umi_studio_developer_workbench_workspace_state(workbench);

    if (argc >= 3) {
        memset(&context, 0, sizeof(context));
        context.struct_size = (uint32_t)sizeof(context);
        context.api_version = UMI_DEVELOPER_CONTEXT_API_VERSION;
        (void)snprintf(context.project_id, sizeof(context.project_id), "%s", argv[2]);
        if (argc >= 4) {
            (void)snprintf(context.workspace_directory,
                           sizeof(context.workspace_directory), "%s", argv[3]);
        }
        status = umi_developer_runtime_set_context(
            umi_studio_developer_workspace_state_centre_runtime(centre), &context);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_developer_workspace_state_centre_save(centre, argv[1]);
        }
    } else if (umi_fs_exists(argv[1]) != 0) {
        status = umi_studio_developer_workspace_state_centre_load(centre, argv[1]);
    } else {
        status = UMI_STATUS_NOT_FOUND;
    }

    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Developer state operation failed: %s\n",
                umi_status_text(status));
        umi_studio_developer_workbench_destroy(workbench);
        return 1;
    }

    status = umi_studio_developer_workspace_state_centre_snapshot(centre, &snapshot);
    if (status == UMI_STATUS_OK) {
        print_state(&snapshot);
    }
    umi_studio_developer_workbench_destroy(workbench);
    return status == UMI_STATUS_OK ? 0 : 1;
}

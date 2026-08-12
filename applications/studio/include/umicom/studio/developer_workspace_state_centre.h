/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_workspace_state_centre.h
 *
 * PURPOSE:
 *   Manage persistent Studio developer context and workflow preferences using
 *   the reusable Framework developer workspace-state contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEVELOPER_WORKSPACE_STATE_CENTRE_H
#define UMICOM_STUDIO_DEVELOPER_WORKSPACE_STATE_CENTRE_H

#include <stdint.h>
#include "umicom/developer/developer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeveloperWorkspaceStateCentre UmiStudioDeveloperWorkspaceStateCentre;

typedef struct UmiStudioDeveloperWorkspaceStateCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char title[256];
    char summary[512];
    UmiDeveloperWorkspaceStateSnapshot state;
    uint64_t revision;
    int available;
} UmiStudioDeveloperWorkspaceStateCentreSnapshot;

UmiStatus umi_studio_developer_workspace_state_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperWorkspaceStateCentre **out_centre);
void umi_studio_developer_workspace_state_centre_destroy(
    UmiStudioDeveloperWorkspaceStateCentre *centre);
UmiStatus umi_studio_developer_workspace_state_centre_snapshot(
    UmiStudioDeveloperWorkspaceStateCentre *centre,
    UmiStudioDeveloperWorkspaceStateCentreSnapshot *out_snapshot);
UmiStatus umi_studio_developer_workspace_state_centre_capture_context(
    UmiStudioDeveloperWorkspaceStateCentre *centre);
UmiStatus umi_studio_developer_workspace_state_centre_set_preferences(
    UmiStudioDeveloperWorkspaceStateCentre *centre,
    const char *last_task_id,
    const char *last_launch_profile_id,
    int configure_before_build,
    int build_before_run,
    int test_after_build);
UmiStatus umi_studio_developer_workspace_state_centre_save(
    UmiStudioDeveloperWorkspaceStateCentre *centre,
    const char *path);
UmiStatus umi_studio_developer_workspace_state_centre_load(
    UmiStudioDeveloperWorkspaceStateCentre *centre,
    const char *path);
UmiDeveloperRuntime *umi_studio_developer_workspace_state_centre_runtime(
    UmiStudioDeveloperWorkspaceStateCentre *centre);

#ifdef __cplusplus
}
#endif
#endif

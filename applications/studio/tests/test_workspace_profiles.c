/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_workspace_profiles.c
 *
 * PURPOSE:
 *   Verify that Studio registers and applies every professional workspace
 *   profile through the live Framework workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/workspace_profiles.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiWorkbenchSnapshot snapshot;
    UmiUiWorkbenchState state;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.profile-test", commands,
                                   &workbench) == UMI_STATUS_OK);
    assert(umi_studio_workspace_profiles_register(workbench) == UMI_STATUS_OK);

    assert(umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.workspace_profile_count ==
           UMI_STUDIO_WORKSPACE_PROFILE_COUNT);
    assert(strcmp(snapshot.active_workspace_profile,
                  UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP) == 0);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_FOCUS) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    assert(!state.sidebar_visible && !state.auxiliary_sidebar_visible &&
           !state.bottom_panel_visible);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_TRADING) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    assert(state.sidebar_visible && state.auxiliary_sidebar_visible &&
           state.bottom_panel_visible);
    assert(state.bottom_panel_size == 300);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_SOURCE_CONTROL) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    assert(state.sidebar_visible && state.auxiliary_sidebar_visible &&
           state.bottom_panel_visible);
    assert(state.bottom_panel_size == 320);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_TESTING) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    assert(state.sidebar_visible && state.auxiliary_sidebar_visible &&
           state.bottom_panel_visible);
    assert(state.bottom_panel_size == 320);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_BUILD) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) == UMI_STATUS_OK);
    assert(state.sidebar_visible && state.auxiliary_sidebar_visible &&
           state.bottom_panel_visible);
    assert(state.bottom_panel_size == 340);

    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}

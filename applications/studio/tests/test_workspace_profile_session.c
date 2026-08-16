/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_workspace_profile_session.c
 *
 * PURPOSE:
 *   Verify that a protected custom layout survives Studio's normal session
 *   save/restore lifecycle and is available before its active ID is applied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"
#include "umicom/studio/appearance_centre.h"
#include "umicom/studio/workbench.h"
#include "umicom/studio/workspace_profiles.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiCommandRegistry *first_commands = NULL;
    UmiCommandRegistry *second_commands = NULL;
    UmiUiWorkbench *first = NULL;
    UmiUiWorkbench *second = NULL;
    UmiSessionStore *session = NULL;
    UmiUiWorkbenchState state;
    UmiUiWorkspaceProfileSnapshot restored;
    char profile_id[UMI_UI_ID_CAPACITY];

    assert(umi_command_registry_create(&first_commands) == UMI_STATUS_OK);
    assert(umi_command_registry_create(&second_commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.first", first_commands, &first) ==
           UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.second", second_commands, &second) ==
           UMI_STATUS_OK);
    assert(umi_studio_workspace_profiles_register(first) == UMI_STATUS_OK);
    assert(umi_studio_workspace_profiles_register(second) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(first) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(second) == UMI_STATUS_OK);

    assert(umi_ui_workbench_state_snapshot(first, &state) == UMI_STATUS_OK);
    state.sidebar_size = 314;
    state.auxiliary_sidebar_size = 418;
    state.bottom_panel_size = 286;
    state.active_workspace_profile[0] = '\0';
    assert(umi_ui_workbench_state_apply(first, &state) == UMI_STATUS_OK);
    assert(umi_ui_workbench_save_workspace_profile(
               first, "Persistent Review", "Restored between sessions",
               profile_id, sizeof(profile_id)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_set_workspace_profile_locked(first, profile_id, 1) ==
           UMI_STATUS_OK);

    assert(umi_session_store_create(&session) == UMI_STATUS_OK);
    assert(umi_studio_workbench_save_session(first, session) == UMI_STATUS_OK);
    assert(umi_studio_workbench_restore_session(second, session) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_count(
               umi_ui_workbench_workspace_profiles(second)) ==
           UMI_STUDIO_WORKSPACE_PROFILE_COUNT + 1U);
    assert(umi_ui_workspace_profile_model_find(
               umi_ui_workbench_workspace_profiles(second), profile_id,
               &restored) == UMI_STATUS_OK);
    assert(restored.active && restored.locked && !restored.built_in);
    assert(restored.sidebar_size == 314);
    assert(restored.auxiliary_sidebar_size == 418);
    assert(restored.bottom_panel_size == 286);
    assert(strcmp(restored.label, "Persistent Review") == 0);

    umi_session_store_destroy(session);
    umi_ui_workbench_destroy(second);
    umi_ui_workbench_destroy(first);
    umi_command_registry_destroy(second_commands);
    umi_command_registry_destroy(first_commands);
    return EXIT_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_appearance_session.c
 *
 * PURPOSE:
 *   Verify that the selected appearance profile survives a Studio session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/appearance_centre.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *first = NULL;
    UmiUiWorkbench *second = NULL;
    UmiSessionStore *session = NULL;
    UmiUiAppearanceProfile active;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.appearance-first", commands,
                                   &first) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.appearance-second", commands,
                                   &second) == UMI_STATUS_OK);
    assert(umi_session_store_create(&session) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(first) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(second) == UMI_STATUS_OK);
    assert(umi_studio_appearance_activate(
               first, UMI_STUDIO_APPEARANCE_HIGH_CONTRAST) == UMI_STATUS_OK);
    assert(umi_studio_appearance_save(first, session) == UMI_STATUS_OK);
    assert(umi_studio_appearance_restore(second, session) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_active(
               umi_ui_workbench_appearance(second), &active) == UMI_STATUS_OK);
    assert(strcmp(active.profile_id,
                  UMI_STUDIO_APPEARANCE_HIGH_CONTRAST) == 0);
    assert(active.reduce_motion);

    umi_session_store_destroy(session);
    umi_ui_workbench_destroy(second);
    umi_ui_workbench_destroy(first);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}

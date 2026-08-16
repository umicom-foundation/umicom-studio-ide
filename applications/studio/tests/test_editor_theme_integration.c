/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_editor_theme_integration.c
 *
 * PURPOSE:
 *   Verify Studio appearances project into the Framework editor theme contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/appearance_centre.h"
#include "umicom/ui/editor_theme.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiAppearanceProfile appearance;
    UmiUiEditorThemeSnapshot editor_theme;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.editor-theme-test", commands,
                                   &workbench) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(workbench) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_active(
               umi_ui_workbench_appearance(workbench), &appearance) ==
           UMI_STATUS_OK);
    assert(umi_ui_editor_theme_resolve(&appearance, &editor_theme) ==
           UMI_STATUS_OK);
    assert(editor_theme.mode == UMI_UI_THEME_MODE_DARK);
    assert(strcmp(editor_theme.current_line, "#FFFFFF") != 0);
    assert(strcmp(editor_theme.background, appearance.editor_background) == 0);

    assert(umi_studio_appearance_activate(
               workbench, UMI_STUDIO_APPEARANCE_LIGHT) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_active(
               umi_ui_workbench_appearance(workbench), &appearance) ==
           UMI_STATUS_OK);
    assert(umi_ui_editor_theme_resolve(&appearance, &editor_theme) ==
           UMI_STATUS_OK);
    assert(editor_theme.mode == UMI_UI_THEME_MODE_LIGHT);
    assert(strcmp(editor_theme.preferred_scheme_ids[0], "Adwaita") == 0);

    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_appearance_centre.c
 *
 * PURPOSE:
 *   Verify Studio's protected branded theme family and context projection.
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
    UmiUiWorkbench *workbench = NULL;
    UmiUiAppearanceProfile profile;
    UmiUiContextSnapshot context;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.appearance-test", commands,
                                   &workbench) == UMI_STATUS_OK);
    assert(umi_studio_appearance_register(workbench) == UMI_STATUS_OK);
    assert(umi_ui_appearance_model_count(
               umi_ui_workbench_appearance(workbench)) == 4U);
    assert(umi_ui_appearance_model_active(
               umi_ui_workbench_appearance(workbench), &profile) ==
           UMI_STATUS_OK);
    assert(strcmp(profile.profile_id, UMI_STUDIO_APPEARANCE_DARK) == 0);
    assert(strcmp(profile.brand_name, "Umicom") == 0);
    assert(strcmp(profile.accent, "#C84C55") == 0);
    assert(umi_ui_appearance_model_remove(
               umi_ui_workbench_appearance(workbench),
               UMI_STUDIO_APPEARANCE_DARK) == UMI_STATUS_PERMISSION_DENIED);

    assert(umi_studio_appearance_set_brand_resources(
               workbench, "branding/umicom-logo.svg",
               "branding/umicom-icon.svg") == UMI_STATUS_OK);
    assert(umi_ui_context_get(umi_ui_workbench_context(workbench),
                              "studio.brand.logo-path", &context) ==
           UMI_STATUS_OK);
    assert(strcmp(context.string_value, "branding/umicom-logo.svg") == 0);

    assert(umi_studio_appearance_activate(
               workbench, UMI_STUDIO_APPEARANCE_LIGHT) == UMI_STATUS_OK);
    assert(umi_ui_context_get(umi_ui_workbench_context(workbench),
                              "studio.ui.theme", &context) == UMI_STATUS_OK);
    assert(strcmp(context.string_value, UMI_STUDIO_APPEARANCE_LIGHT) == 0);
    assert(umi_ui_context_get(umi_ui_workbench_context(workbench),
                              "studio.ui.font.editor", &context) ==
           UMI_STATUS_OK);
    assert(strstr(context.string_value, "Cascadia Code") != NULL);

    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}

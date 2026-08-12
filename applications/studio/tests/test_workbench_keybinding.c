/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_keybinding.c
 *
 * PURPOSE:
 *   Verify a Studio keybinding resolves to one canonical command plus argument.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui.h"
#include "umicom/studio/workbench_commands.h"
#include "umicom/studio/workbench_shell_catalogue.h"
#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiKeybindingResolution resolution;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Shift+F",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE) == 0);
    assert(strcmp(resolution.argument,
                  UMI_STUDIO_ACTIVITY_SEARCH) == 0);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_installer_centre.c
 *
 * PURPOSE:
 *   Verify Studio installer defaults and generator selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/installer_centre.h"

int main(void)
{
    UmiStudioInstallerCentre centre;
    UmiInstallerPlan *windows;
    assert(umi_studio_installer_centre_init(&centre, "0.23.0") ==
           UMI_STATUS_OK);
    assert(umi_studio_installer_centre_ready(&centre));
    windows = umi_studio_installer_centre_plan(
        &centre, UMI_INSTALLER_WINDOWS);
    assert(windows != NULL);
    assert(windows->create_start_menu_shortcut);
    assert(strcmp(umi_installer_plan_generator(windows), "NSIS") == 0);
    return 0;
}

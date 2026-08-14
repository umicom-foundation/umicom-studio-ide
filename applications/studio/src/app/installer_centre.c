/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/installer_centre.c
 *
 * PURPOSE:
 *   Configure Studio Windows and Linux installer-generation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/installer_centre.h"
#include <string.h>

UmiStatus umi_studio_installer_centre_init(UmiStudioInstallerCentre *centre,
                                               const char *version)
{
    UmiStatus status;
    if (centre == NULL || version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(centre, 0, sizeof(*centre));
    status = umi_installer_plan_init(
        &centre->windows, "Umicom Studio", "Umicom Foundation", version,
        "Umicom Studio", "bin/umicom-studio-console.exe",
        UMI_INSTALLER_WINDOWS, UMI_INSTALL_SCOPE_USER);
    if (status != UMI_STATUS_OK) return status;
    umi_installer_plan_set_shortcuts(&centre->windows, 1, 0);
    status = umi_installer_plan_init(
        &centre->linux, "Umicom Studio", "Umicom Foundation", version,
        "/opt/umicom-studio", "bin/umicom-studio-console",
        UMI_INSTALLER_LINUX, UMI_INSTALL_SCOPE_USER);
    if (status != UMI_STATUS_OK) return status;
    umi_installer_plan_set_shortcuts(&centre->linux, 0, 0);
    return UMI_STATUS_OK;
}

UmiInstallerPlan *umi_studio_installer_centre_plan(
    UmiStudioInstallerCentre *centre,
    UmiInstallerPlatform platform)
{
    if (centre == NULL) return NULL;
    if (platform == UMI_INSTALLER_WINDOWS) return &centre->windows;
    if (platform == UMI_INSTALLER_LINUX) return &centre->linux;
    return NULL;
}

int umi_studio_installer_centre_ready(
    const UmiStudioInstallerCentre *centre)
{
    return centre != NULL &&
           umi_installer_plan_validate(&centre->windows) == UMI_STATUS_OK &&
           umi_installer_plan_validate(&centre->linux) == UMI_STATUS_OK;
}

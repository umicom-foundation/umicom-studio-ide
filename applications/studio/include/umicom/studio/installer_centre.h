/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/installer_centre.h
 *
 * PURPOSE:
 *   Configure Studio Windows and Linux installer-generation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_INSTALLER_CENTRE_H
#define UMICOM_STUDIO_INSTALLER_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioInstallerCentre {
    UmiInstallerPlan windows;
    UmiInstallerPlan linux;
} UmiStudioInstallerCentre;

UmiStatus umi_studio_installer_centre_init(UmiStudioInstallerCentre *centre,
                                               const char *version);
UmiInstallerPlan *umi_studio_installer_centre_plan(
    UmiStudioInstallerCentre *centre,
    UmiInstallerPlatform platform);
int umi_studio_installer_centre_ready(
    const UmiStudioInstallerCentre *centre);

#ifdef __cplusplus
}
#endif
#endif

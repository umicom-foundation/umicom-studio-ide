/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/installer_centre_v2.h
 *
 * PURPOSE:
 *   Configure Studio Windows and Linux installer-generation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_INSTALLER_CENTRE_V2_H
#define UMICOM_STUDIO_INSTALLER_CENTRE_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioInstallerCentreV2 {
    UmiInstallerPlanV2 windows;
    UmiInstallerPlanV2 linux;
} UmiStudioInstallerCentreV2;

UmiStatus umi_studio_installer_centre_v2_init(UmiStudioInstallerCentreV2 *centre,
                                               const char *version);
UmiInstallerPlanV2 *umi_studio_installer_centre_v2_plan(
    UmiStudioInstallerCentreV2 *centre,
    UmiInstallerPlatformV2 platform);
int umi_studio_installer_centre_v2_ready(
    const UmiStudioInstallerCentreV2 *centre);

#ifdef __cplusplus
}
#endif
#endif

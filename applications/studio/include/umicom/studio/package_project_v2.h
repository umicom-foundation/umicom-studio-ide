/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/package_project_v2.h
 *
 * PURPOSE:
 *   Build Windows and Linux package manifests for the Studio product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PACKAGE_PROJECT_V2_H
#define UMICOM_STUDIO_PACKAGE_PROJECT_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioPackageProjectV2 {
    UmiPackageManifestV2 windows;
    UmiPackageManifestV2 linux;
} UmiStudioPackageProjectV2;

UmiStatus umi_studio_package_project_v2_init(UmiStudioPackageProjectV2 *project,
                                              const char *version);
const UmiPackageManifestV2 *umi_studio_package_project_v2_manifest(
    const UmiStudioPackageProjectV2 *project,
    UmiInstallerPlatformV2 platform);
int umi_studio_package_project_v2_ready(const UmiStudioPackageProjectV2 *project);

#ifdef __cplusplus
}
#endif
#endif

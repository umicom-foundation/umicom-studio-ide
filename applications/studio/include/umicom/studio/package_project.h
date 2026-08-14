/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/package_project.h
 *
 * PURPOSE:
 *   Build Windows and Linux package manifests for the Studio product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PACKAGE_PROJECT_H
#define UMICOM_STUDIO_PACKAGE_PROJECT_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioPackageProject {
    UmiPackageManifest windows;
    UmiPackageManifest linux;
} UmiStudioPackageProject;

UmiStatus umi_studio_package_project_init(UmiStudioPackageProject *project,
                                              const char *version);
const UmiPackageManifest *umi_studio_package_project_manifest(
    const UmiStudioPackageProject *project,
    UmiInstallerPlatform platform);
int umi_studio_package_project_ready(const UmiStudioPackageProject *project);

#ifdef __cplusplus
}
#endif
#endif

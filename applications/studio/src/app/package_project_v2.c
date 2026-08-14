/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/package_project_v2.c
 *
 * PURPOSE:
 *   Build Windows and Linux package manifests for the Studio product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/package_project_v2.h"
#include <string.h>

static UmiStatus umi_studio_package_add_common(UmiPackageManifestV2 *manifest,
                                                const char *source,
                                                const char *destination)
{
    UmiStatus status = umi_package_manifest_v2_add(
        manifest, source, destination, UMI_PACKAGE_FILE_EXECUTABLE_V2, 1);
    if (status != UMI_STATUS_OK) return status;
    status = umi_package_manifest_v2_add(
        manifest, "LICENSE", "share/licenses/umicom-studio/LICENSE",
        UMI_PACKAGE_FILE_LICENCE_V2, 1);
    if (status != UMI_STATUS_OK) return status;
    return umi_package_manifest_v2_add(
        manifest, "config", "share/umicom/config",
        UMI_PACKAGE_FILE_CONFIGURATION_V2, 0);
}

UmiStatus umi_studio_package_project_v2_init(UmiStudioPackageProjectV2 *project,
                                              const char *version)
{
    UmiStatus status;
    if (project == NULL || version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(project, 0, sizeof(*project));
    status = umi_package_manifest_v2_init(
        &project->windows, "umicom-studio", version,
        "bin/umicom-studio-console.exe", UMI_PACKAGE_WINDOWS_SETUP);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_package_add_common(
        &project->windows, "build/bin/umicom-studio-console.exe",
        "bin/umicom-studio-console.exe");
    if (status != UMI_STATUS_OK) return status;
    status = umi_package_manifest_v2_init(
        &project->linux, "umicom-studio", version,
        "bin/umicom-studio-console", UMI_PACKAGE_TAR_GZ);
    if (status != UMI_STATUS_OK) return status;
    return umi_studio_package_add_common(
        &project->linux, "build/bin/umicom-studio-console",
        "bin/umicom-studio-console");
}

const UmiPackageManifestV2 *umi_studio_package_project_v2_manifest(
    const UmiStudioPackageProjectV2 *project,
    UmiInstallerPlatformV2 platform)
{
    if (project == NULL) return NULL;
    if (platform == UMI_INSTALLER_WINDOWS_V2) return &project->windows;
    if (platform == UMI_INSTALLER_LINUX_V2) return &project->linux;
    return NULL;
}

int umi_studio_package_project_v2_ready(const UmiStudioPackageProjectV2 *project)
{
    return project != NULL &&
           umi_package_manifest_v2_validate(&project->windows) == UMI_STATUS_OK &&
           umi_package_manifest_v2_validate(&project->linux) == UMI_STATUS_OK;
}

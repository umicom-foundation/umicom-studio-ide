/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_package_project_v2.c
 *
 * PURPOSE:
 *   Verify Studio Windows and Linux package manifests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/package_project_v2.h"

int main(void)
{
    UmiStudioPackageProjectV2 project;
    const UmiPackageManifestV2 *windows;
    assert(umi_studio_package_project_v2_init(&project, "0.23.0") ==
           UMI_STATUS_OK);
    assert(umi_studio_package_project_v2_ready(&project));
    windows = umi_studio_package_project_v2_manifest(
        &project, UMI_INSTALLER_WINDOWS_V2);
    assert(windows != NULL);
    assert(windows->format == UMI_PACKAGE_WINDOWS_SETUP);
    assert(windows->count == 3U);
    return 0;
}

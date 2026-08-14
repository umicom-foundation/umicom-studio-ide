/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_dependency_bundle_v2.c
 *
 * PURPOSE:
 *   Verify dependency classification and portable bundle composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/bundle_designer_v2.h"

int main(void)
{
    UmiStudioPackageProjectV2 project;
    UmiStudioDependencyCentreV2 dependencies;
    UmiStudioBundleDesignerV2 bundle;
    assert(umi_studio_package_project_v2_init(&project, "0.23.0") ==
           UMI_STATUS_OK);
    umi_studio_dependency_centre_v2_init(&dependencies);
    assert(umi_studio_dependency_centre_v2_add(
               &dependencies, "kernel32.dll", "") == UMI_STATUS_OK);
    assert(umi_studio_dependency_centre_v2_add(
               &dependencies, "libgtk-4-1.dll", "bin/libgtk-4-1.dll") ==
           UMI_STATUS_OK);
    assert(umi_studio_dependency_centre_v2_ready(&dependencies));
    assert(umi_studio_bundle_designer_v2_init(
               &bundle, &project.windows, "windows-x64", "stage",
               "dist/umicom-studio.zip") == UMI_STATUS_OK);
    assert(umi_studio_bundle_designer_v2_apply_dependencies(
               &bundle, &dependencies) == UMI_STATUS_OK);
    assert(umi_studio_bundle_designer_v2_ready(&bundle));
    assert(umi_portable_bundle_v2_file_count(&bundle.plan) == 4U);
    return 0;
}

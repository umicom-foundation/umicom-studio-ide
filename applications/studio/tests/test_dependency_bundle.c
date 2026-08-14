/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_dependency_bundle.c
 *
 * PURPOSE:
 *   Verify dependency classification and portable bundle composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/bundle_designer.h"

int main(void)
{
    UmiStudioPackageProject project;
    UmiStudioDependencyCentre dependencies;
    UmiStudioBundleDesigner bundle;
    assert(umi_studio_package_project_init(&project, "0.23.0") ==
           UMI_STATUS_OK);
    umi_studio_dependency_centre_init(&dependencies);
    assert(umi_studio_dependency_centre_add(
               &dependencies, "kernel32.dll", "") == UMI_STATUS_OK);
    assert(umi_studio_dependency_centre_add(
               &dependencies, "libgtk-4-1.dll", "bin/libgtk-4-1.dll") ==
           UMI_STATUS_OK);
    assert(umi_studio_dependency_centre_ready(&dependencies));
    assert(umi_studio_bundle_designer_init(
               &bundle, &project.windows, "windows-x64", "stage",
               "dist/umicom-studio.zip") == UMI_STATUS_OK);
    assert(umi_studio_bundle_designer_apply_dependencies(
               &bundle, &dependencies) == UMI_STATUS_OK);
    assert(umi_studio_bundle_designer_ready(&bundle));
    assert(umi_portable_bundle_file_count(&bundle.plan) == 4U);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_runtime_binding.c
 *
 * PURPOSE:
 *   Verify the Studio developer runtime binds to the exact Framework services
 *   already owned by the existing Studio centres instead of creating a second
 *   disconnected project/language/debug/source-control/test model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/studio/developer_workbench.h"

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiDeveloperRuntime *runtime;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    runtime = umi_studio_developer_workbench_runtime(workbench);
    assert(runtime != NULL);

    assert(umi_developer_runtime_projects(runtime) ==
           umi_studio_project_centre_service(
               umi_studio_developer_workbench_projects(workbench)));
    assert(umi_developer_runtime_language(runtime) ==
           umi_studio_language_intelligence_centre_service(
               umi_studio_developer_workbench_language(workbench)));
    assert(umi_developer_runtime_debug(runtime) ==
           umi_studio_debug_centre_service(
               umi_studio_developer_workbench_debug(workbench)));
    assert(umi_developer_runtime_source_control(runtime) ==
           umi_studio_source_control_centre_service(
               umi_studio_developer_workbench_source_control(workbench)));
    assert(umi_developer_runtime_tests(runtime) ==
           umi_studio_test_explorer_centre_service(
               umi_studio_developer_workbench_tests(workbench)));
    assert(umi_developer_runtime_workbench(runtime) ==
           umi_studio_developer_workbench_services(workbench));

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

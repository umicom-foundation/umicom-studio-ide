/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_pipeline_centre.c
 *
 * PURPOSE:
 *   Verify Studio exposes the Framework developer pipeline and can create a
 *   dependency-aware CMake plan without duplicating pipeline state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/developer_workbench.h"

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperPipelineCentre *centre;
    UmiStudioDeveloperPipelineCentreSnapshot snapshot;
    UmiDeveloperCMakePlanRequest request;
    UmiDeveloperCMakePlanSnapshot plan;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);

    centre = umi_studio_developer_workbench_pipeline(workbench);
    assert(centre != NULL);

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    request.plan_id = "studio.test";
    request.project_id = "studio";
    request.configuration_id = "Debug";
    request.source_directory = ".";
    request.build_directory = "build/test";
    request.generator = "Ninja";
    request.include_configure = 1;
    request.include_build = 1;
    request.include_tests = 1;

    assert(umi_studio_developer_pipeline_centre_prepare_cmake(
        centre, &request, &plan) == UMI_STATUS_OK);
    assert(plan.operation_count == 3U);

    assert(umi_studio_developer_pipeline_centre_snapshot(
        centre, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.available == 1);
    assert(snapshot.pipeline.operation_count == 3U);
    assert(snapshot.pipeline.dependency_count == 2U);
    assert(snapshot.pipeline.ready_count == 1U);
    assert(snapshot.item_count >= 3U);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

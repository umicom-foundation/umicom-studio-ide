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
#include <stdint.h>
#include <string.h>

#include "umicom/studio/developer_workbench.h"

static void add_project(UmiStudioDeveloperWorkbench *workbench)
{
    UmiProjectWorkspace *workspace =
        umi_studio_project_centre_service(
            umi_studio_developer_workbench_projects(workbench));
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectConfigurationSnapshot configuration = {0};
    UmiProjectTargetSnapshot target = {0};
    UmiProjectTaskSnapshot build = {0};

    project.struct_size=(uint32_t)sizeof(project);
    project.api_version=UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id,"studio"); strcpy(project.name,"Umicom Studio");
    strcpy(project.root_uri,"."); project.enabled=1;
    assert(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace),&project)==UMI_STATUS_OK);

    configuration.struct_size=(uint32_t)sizeof(configuration);
    configuration.api_version=UMI_PROJECT_CONFIGURATION_API_VERSION;
    strcpy(configuration.id,"debug"); strcpy(configuration.project_id,"studio");
    strcpy(configuration.name,"Debug"); configuration.active=1;
    assert(umi_project_configuration_registry_upsert(
        umi_project_workspace_configuration(workspace),&configuration)==UMI_STATUS_OK);

    target.struct_size=(uint32_t)sizeof(target);
    target.api_version=UMI_PROJECT_TARGET_API_VERSION;
    strcpy(target.id,"studio-console"); strcpy(target.project_id,"studio");
    strcpy(target.name,"Studio Console"); target.enabled=1; target.default_target=1;
    assert(umi_project_target_registry_upsert(
        umi_project_workspace_target(workspace),&target)==UMI_STATUS_OK);

    build.struct_size=(uint32_t)sizeof(build);
    build.api_version=UMI_PROJECT_TASK_API_VERSION;
    strcpy(build.id,"build"); strcpy(build.project_id,"studio");
    strcpy(build.label,"Build"); strcpy(build.command,"cmake --build build");
    strcpy(build.group,"build"); build.enabled=1; build.default_task=1;
    assert(umi_project_task_registry_upsert(
        umi_project_workspace_task(workspace),&build)==UMI_STATUS_OK);
}

static void verify_delivery_contributions(void)
{
    size_t index;
    size_t comparison;
    assert(umi_studio_build_delivery_command_contribution_count() ==
           umi_build_delivery_command_count());
    assert(umi_studio_build_delivery_view_contribution_count() ==
           umi_build_delivery_view_count());
    for (index = 0U;
         index < umi_studio_build_delivery_command_contribution_count();
         ++index) {
        const UmiStudioBuildDeliveryCommandContribution *contribution =
            umi_studio_build_delivery_command_contribution_at(index);
        assert(contribution != NULL);
        assert(umi_build_delivery_command_find(
                   contribution->framework_command_id) != NULL);
        for (comparison = index + 1U;
             comparison <
                 umi_studio_build_delivery_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_build_delivery_command_contribution_at(
                           comparison)->framework_command_id) != 0);
        }
    }
    for (index = 0U;
         index < umi_studio_build_delivery_view_contribution_count();
         ++index) {
        const UmiStudioBuildDeliveryViewContribution *contribution =
            umi_studio_build_delivery_view_contribution_at(index);
        assert(contribution != NULL);
        assert(umi_build_delivery_view_find(
                   contribution->framework_view_id) != NULL);
    }
}

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperPipelineCentre *centre;
    UmiStudioDeveloperPipelineCentreSnapshot snapshot;
    UmiDeveloperCMakePlanRequest request;
    UmiDeveloperCMakePlanSnapshot plan;
    UmiDeveloperProjectWorkflowRequest project_request = {0};
    UmiDeveloperProjectWorkflowSnapshot project_workflow;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_pipeline(workbench);
    assert(centre != NULL);
    assert(umi_studio_developer_pipeline_centre_delivery(centre) != NULL);
    assert(umi_studio_developer_pipeline_centre_delivery_tasks(centre) != NULL);
    assert(umi_studio_developer_pipeline_centre_deployment_targets(centre) != NULL);

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
    assert(snapshot.delivery.available != 0);
    assert(snapshot.delivery_command_count == 36U);
    assert(snapshot.delivery_view_count == 8U);
    verify_delivery_contributions();

    /*
     * A second workbench keeps the project-aware preset independent of the
     * CMake-plan operations above.
     */
    umi_studio_developer_workbench_destroy(workbench);
    workbench = NULL;
    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    add_project(workbench);
    centre = umi_studio_developer_workbench_pipeline(workbench);

    project_request.struct_size=(uint32_t)sizeof(project_request);
    project_request.api_version=UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    project_request.preset=UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD;
    project_request.workflow_id="studio.build";
    project_request.project_id="studio";
    assert(umi_studio_developer_pipeline_centre_prepare_project_workflow(
        centre,&project_request,&project_workflow)==UMI_STATUS_OK);
    assert(project_workflow.validation.valid!=0);
    assert(project_workflow.workflow.operation_count==1U);
    assert(project_workflow.task_count==1U);
    assert(umi_studio_developer_pipeline_centre_snapshot(
        centre,&snapshot)==UMI_STATUS_OK);
    assert(snapshot.has_project_workflow!=0);
    assert(snapshot.last_project_workflow.preset==
           UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD);
    assert(snapshot.delivery.available != 0);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_pipeline_centre.h
 *
 * PURPOSE:
 *   Expose the reusable Framework developer pipeline as a Studio workbench
 *   centre with CMake planning and operation execution entry points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This centre does not duplicate Framework operation state.  It borrows the
 * UmiDeveloperRuntime owned by the Studio developer workbench and provides the
 * small product-facing surface that command handlers, panels and future GTK4
 * views need.
 */
#ifndef UMICOM_STUDIO_DEVELOPER_PIPELINE_CENTRE_H
#define UMICOM_STUDIO_DEVELOPER_PIPELINE_CENTRE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/build/delivery_platform.h"
#include "umicom/developer/developer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_BUILD_DELIVERY_CONTRIBUTION_API_VERSION 1U

typedef struct UmiStudioDeveloperPipelineCentre UmiStudioDeveloperPipelineCentre;

typedef struct UmiStudioBuildDeliveryCommandContribution {
    uint32_t structure_size;
    uint32_t api_version;
    const char *framework_command_id;
    const char *menu_id;
    const char *group_id;
    int order;
} UmiStudioBuildDeliveryCommandContribution;

typedef struct UmiStudioBuildDeliveryViewContribution {
    uint32_t structure_size;
    uint32_t api_version;
    const char *framework_view_id;
    const char *default_region;
    int order;
    int closable;
    int movable;
} UmiStudioBuildDeliveryViewContribution;

typedef struct UmiStudioDeveloperPipelineCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char title[256];
    char summary[512];
    UmiDeveloperContextSnapshot context;
    UmiDeveloperPipelineSnapshot pipeline;
    UmiDeveloperJournalSnapshot journal;
    UmiBuildDeliveryPipelineSnapshot delivery;
    uint64_t revision;
    size_t item_count;
    size_t delivery_command_count;
    size_t delivery_view_count;
    int available;
    UmiDeveloperProjectWorkflowSnapshot last_project_workflow;
    int has_project_workflow;
} UmiStudioDeveloperPipelineCentreSnapshot;

UmiStatus umi_studio_developer_pipeline_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperPipelineCentre **out_centre);
void umi_studio_developer_pipeline_centre_destroy(
    UmiStudioDeveloperPipelineCentre *centre);
UmiStatus umi_studio_developer_pipeline_centre_snapshot(
    UmiStudioDeveloperPipelineCentre *centre,
    UmiStudioDeveloperPipelineCentreSnapshot *out_snapshot);
UmiStatus umi_studio_developer_pipeline_centre_prepare_cmake(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan);
UmiStatus umi_studio_developer_pipeline_centre_execute_next(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result);
UmiStatus umi_studio_developer_pipeline_centre_execute_next_process(
    UmiStudioDeveloperPipelineCentre *centre,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result);
UmiStatus umi_studio_developer_pipeline_centre_submit_task(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan);
UmiStatus umi_studio_developer_pipeline_centre_submit_launch(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan);
UmiStatus umi_studio_developer_pipeline_centre_submit_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperWorkflowRequest *request,
    UmiDeveloperWorkflowSnapshot *out_workflow);
UmiStatus umi_studio_developer_pipeline_centre_prepare_project_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow);
UmiStatus umi_studio_developer_pipeline_centre_execute_batch(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperExecutor *executor,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot);
UmiStatus umi_studio_developer_pipeline_centre_execute_batch_process(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot);
UmiDeveloperRuntime *umi_studio_developer_pipeline_centre_runtime(
    UmiStudioDeveloperPipelineCentre *centre);
UmiBuildTaskRegistry *umi_studio_developer_pipeline_centre_delivery_tasks(
    UmiStudioDeveloperPipelineCentre *centre);
UmiBuildDeploymentTargetRegistry *
umi_studio_developer_pipeline_centre_deployment_targets(
    UmiStudioDeveloperPipelineCentre *centre);
UmiBuildDeliveryPipeline *umi_studio_developer_pipeline_centre_delivery(
    UmiStudioDeveloperPipelineCentre *centre);

size_t umi_studio_build_delivery_command_contribution_count(void);
const UmiStudioBuildDeliveryCommandContribution *
umi_studio_build_delivery_command_contribution_at(size_t position);
const UmiStudioBuildDeliveryCommandContribution *
umi_studio_build_delivery_command_contribution_find(
    const char *framework_command_id);
size_t umi_studio_build_delivery_view_contribution_count(void);
const UmiStudioBuildDeliveryViewContribution *
umi_studio_build_delivery_view_contribution_at(size_t position);
const UmiStudioBuildDeliveryViewContribution *
umi_studio_build_delivery_view_contribution_find(
    const char *framework_view_id);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_STUDIO_DEVELOPER_PIPELINE_CENTRE_H */

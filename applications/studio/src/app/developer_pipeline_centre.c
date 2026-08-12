/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_pipeline_centre.c
 *
 * PURPOSE:
 *   Implement the Studio developer-pipeline composition over the reusable
 *   Framework developer runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/developer_pipeline_centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioDeveloperPipelineCentre {
    UmiDeveloperRuntime *runtime;
    UmiDeveloperProjectWorkflowSnapshot last_project_workflow;
    uint64_t revision;
    int has_project_workflow;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source == NULL) {
        source = "";
    }

    length = strlen(source);
    if (length >= capacity) {
        length = capacity - 1U;
    }
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
}

UmiStatus umi_studio_developer_pipeline_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperPipelineCentre **out_centre)
{
    UmiStudioDeveloperPipelineCentre *centre;

    if (runtime == NULL || out_centre == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_centre = NULL;
    centre = (UmiStudioDeveloperPipelineCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    centre->runtime = runtime;
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_developer_pipeline_centre_destroy(
    UmiStudioDeveloperPipelineCentre *centre)
{
    /*
     * The runtime is borrowed from UmiStudioDeveloperWorkbench.  Destroying
     * this lightweight centre must never destroy the shared Framework runtime.
     */
    free(centre);
}

UmiStatus umi_studio_developer_pipeline_centre_snapshot(
    UmiStudioDeveloperPipelineCentre *centre,
    UmiStudioDeveloperPipelineCentreSnapshot *out_snapshot)
{
    UmiDeveloperRuntimeSnapshot runtime_snapshot;
    UmiStatus status;

    if (centre == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_runtime_snapshot(centre->runtime, &runtime_snapshot);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    copy_text(out_snapshot->area_id, sizeof(out_snapshot->area_id),
              "studio.developer-pipeline");
    copy_text(out_snapshot->title, sizeof(out_snapshot->title),
              "Developer Pipeline");
    copy_text(out_snapshot->summary, sizeof(out_snapshot->summary),
              "Dependency-aware configure, build, test, run and developer operation orchestration.");
    out_snapshot->context = runtime_snapshot.context;
    out_snapshot->pipeline = runtime_snapshot.pipeline;
    out_snapshot->journal = runtime_snapshot.journal;
    out_snapshot->revision = centre->revision + runtime_snapshot.revision;
    out_snapshot->item_count =
        runtime_snapshot.pipeline.operation_count +
        runtime_snapshot.journal.entry_count;
    out_snapshot->available = 1;
    if (centre->has_project_workflow) {
        out_snapshot->last_project_workflow = centre->last_project_workflow;
        out_snapshot->has_project_workflow = 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_pipeline_centre_prepare_cmake(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperCMakePlanRequest *request,
    UmiDeveloperCMakePlanSnapshot *out_plan)
{
    UmiStatus status;

    if (centre == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_runtime_submit_cmake_plan(
        centre->runtime,
        request,
        out_plan);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_next(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result)
{
    UmiStatus status;

    if (centre == NULL || executor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_runtime_execute_next(
        centre->runtime, executor, out_operation, out_result);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_next_process(
    UmiStudioDeveloperPipelineCentre *centre,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result)
{
    UmiDeveloperExecutor executor;

    if (centre == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_executor_init(&executor, umi_developer_process_execute, NULL);
    return umi_studio_developer_pipeline_centre_execute_next(
        centre, &executor, out_operation, out_result);
}

UmiStatus umi_studio_developer_pipeline_centre_submit_task(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_developer_task_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_submit_launch(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_developer_launch_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_submit_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperWorkflowRequest *request,
    UmiDeveloperWorkflowSnapshot *out_workflow)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_developer_workflow_submit(centre->runtime, request, out_workflow);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_prepare_project_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow)
{
    UmiDeveloperProjectWorkflowSnapshot workflow;
    UmiStatus status;

    if (centre == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_workflow_submit(
        centre->runtime, request, &workflow);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    centre->last_project_workflow = workflow;
    centre->has_project_workflow = 1;
    centre->revision += 1U;
    if (out_workflow != NULL) {
        *out_workflow = workflow;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_batch(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperExecutor *executor,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot)
{
    UmiStatus status;
    if (centre == NULL || executor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_developer_batch_execute(
        centre->runtime, executor, request, out_snapshot);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_batch_process(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot)
{
    UmiStatus status;
    if (centre == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_developer_batch_execute_process(
        centre->runtime, request, out_snapshot);
    if (status == UMI_STATUS_OK) {
        centre->revision += 1U;
    }
    return status;
}

UmiDeveloperRuntime *umi_studio_developer_pipeline_centre_runtime(
    UmiStudioDeveloperPipelineCentre *centre)
{
    return centre != NULL ? centre->runtime : NULL;
}

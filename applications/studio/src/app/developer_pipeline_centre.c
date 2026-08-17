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
    UmiBuildTaskRegistry *delivery_tasks;
    UmiBuildDeploymentTargetRegistry *deployment_targets;
    UmiBuildDeliveryPipeline *delivery;
    UmiDeveloperProjectWorkflowSnapshot last_project_workflow;
    uint64_t revision;
    int has_project_workflow;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

#define COMMAND(id, menu, group, priority)                                \
    { sizeof(UmiStudioBuildDeliveryCommandContribution),                  \
      UMI_STUDIO_BUILD_DELIVERY_CONTRIBUTION_API_VERSION,                 \
      id, menu, group, priority }

static const UmiStudioBuildDeliveryCommandContribution DELIVERY_COMMANDS[] = {
    COMMAND("build.graph.open", "View", "build", 500),
    COMMAND("build.graph.refresh", "Build", "graph", 100),
    COMMAND("build.graph.focus-dirty", "Build", "graph", 110),
    COMMAND("build.graph.force-all", "Build", "graph", 120),
    COMMAND("build.configure", "Build", "lifecycle", 200),
    COMMAND("build.workspace", "Build", "lifecycle", 210),
    COMMAND("build.selected", "Build", "lifecycle", 220),
    COMMAND("build.rebuild", "Build", "lifecycle", 230),
    COMMAND("build.clean", "Build", "lifecycle", 240),
    COMMAND("build.cancel", "Build", "lifecycle", 250),
    COMMAND("task.open", "View", "build", 510),
    COMMAND("task.run", "Tasks", "execution", 100),
    COMMAND("task.run-default", "Tasks", "execution", 110),
    COMMAND("task.rerun", "Tasks", "execution", 120),
    COMMAND("task.retry-failed", "Tasks", "execution", 130),
    COMMAND("task.stop", "Tasks", "execution", 140),
    COMMAND("artifact.open", "View", "build", 520),
    COMMAND("artifact.reveal", "Build", "artifacts", 300),
    COMMAND("artifact.copy-path", "Edit", "artifacts", 300),
    COMMAND("artifact.verify", "Build", "artifacts", 310),
    COMMAND("artifact.select-primary", "Build", "artifacts", 320),
    COMMAND("deployment.targets.open", "View", "deployment", 600),
    COMMAND("deployment.plan", "Deployment", "planning", 100),
    COMMAND("deployment.preview", "Deployment", "planning", 110),
    COMMAND("deployment.approve", "Deployment", "approval", 200),
    COMMAND("deployment.run", "Deployment", "execution", 300),
    COMMAND("deployment.cancel", "Deployment", "execution", 310),
    COMMAND("deployment.rollback", "Deployment", "recovery", 400),
    COMMAND("deployment.history.open", "View", "deployment", 610),
    COMMAND("pipeline.open", "View", "build", 530),
    COMMAND("pipeline.run", "Build", "pipeline", 400),
    COMMAND("pipeline.stop", "Build", "pipeline", 410),
    COMMAND("pipeline.refresh", "Build", "pipeline", 420),
    COMMAND("pipeline.clear", "Build", "pipeline", 430),
    COMMAND("pipeline.copy-summary", "Edit", "pipeline", 410),
    COMMAND("pipeline.export-evidence", "Build", "pipeline", 440)
};

#undef COMMAND

#define VIEW(id, region, priority)                                        \
    { sizeof(UmiStudioBuildDeliveryViewContribution),                     \
      UMI_STUDIO_BUILD_DELIVERY_CONTRIBUTION_API_VERSION,                 \
      id, region, priority, 1, 1 }

static const UmiStudioBuildDeliveryViewContribution DELIVERY_VIEWS[] = {
    VIEW("build.graph", "editor-area", 100),
    VIEW("build.tasks", "left-sidebar", 110),
    VIEW("build.progress", "bottom-panel", 120),
    VIEW("build.artifacts", "bottom-panel", 130),
    VIEW("deployment.targets", "secondary-sidebar", 140),
    VIEW("deployment.plan", "editor-area", 150),
    VIEW("deployment.history", "bottom-panel", 160),
    VIEW("build.delivery-pipeline", "bottom-panel", 170)
};

#undef VIEW

UmiStatus umi_studio_developer_pipeline_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperPipelineCentre **out_centre)
{
    UmiStudioDeveloperPipelineCentre *centre;
    UmiStatus status;
    if (runtime == NULL || out_centre == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_centre = NULL;
    centre = (UmiStudioDeveloperPipelineCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->runtime = runtime;
    centre->revision = 1U;
    status = umi_build_task_registry_create(&centre->delivery_tasks);
    if (status == UMI_STATUS_OK) {
        status = umi_build_deployment_target_registry_create(
            &centre->deployment_targets);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_build_delivery_pipeline_create(
            "studio.build-delivery", centre->delivery_tasks,
            centre->deployment_targets, 2U, &centre->delivery);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_developer_pipeline_centre_destroy(centre);
        return status;
    }
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_developer_pipeline_centre_destroy(
    UmiStudioDeveloperPipelineCentre *centre)
{
    if (centre == NULL) return;
    /* The developer runtime is borrowed from the owning workbench. */
    umi_build_delivery_pipeline_destroy(centre->delivery);
    umi_build_deployment_target_registry_destroy(centre->deployment_targets);
    umi_build_task_registry_destroy(centre->delivery_tasks);
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
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 2U;
    copy_text(out_snapshot->area_id, sizeof(out_snapshot->area_id),
              "studio.developer-pipeline");
    copy_text(out_snapshot->title, sizeof(out_snapshot->title),
              "Build, Tasks and Deployment");
    copy_text(out_snapshot->summary, sizeof(out_snapshot->summary),
              "Framework-owned incremental build graph, dependency tasks, artifact evidence and governed deployment pipeline.");
    out_snapshot->context = runtime_snapshot.context;
    out_snapshot->pipeline = runtime_snapshot.pipeline;
    out_snapshot->journal = runtime_snapshot.journal;
    status = umi_build_delivery_pipeline_snapshot(
        centre->delivery, &out_snapshot->delivery);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->delivery_command_count =
        umi_studio_build_delivery_command_contribution_count();
    out_snapshot->delivery_view_count =
        umi_studio_build_delivery_view_contribution_count();
    out_snapshot->revision = centre->revision + runtime_snapshot.revision +
                             out_snapshot->delivery.revision;
    out_snapshot->item_count =
        runtime_snapshot.pipeline.operation_count +
        runtime_snapshot.journal.entry_count +
        out_snapshot->delivery.tasks.planned_count +
        out_snapshot->delivery.artifacts.artifact_count +
        out_snapshot->delivery.deployment.plan.step_count;
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
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_runtime_submit_cmake_plan(
        centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_next(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperExecutor *executor,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result)
{
    UmiStatus status;
    if (centre == NULL || executor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_runtime_execute_next(
        centre->runtime, executor, out_operation, out_result);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_next_process(
    UmiStudioDeveloperPipelineCentre *centre,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperExecutionResult *out_result)
{
    UmiDeveloperExecutor executor;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_task_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_submit_launch(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_launch_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_submit_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperWorkflowRequest *request,
    UmiDeveloperWorkflowSnapshot *out_workflow)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_workflow_submit(
        centre->runtime, request, out_workflow);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_prepare_project_workflow(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow)
{
    UmiDeveloperProjectWorkflowSnapshot workflow;
    UmiStatus status;
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_project_workflow_submit(
        centre->runtime, request, &workflow);
    if (status != UMI_STATUS_OK) return status;
    centre->last_project_workflow = workflow;
    centre->has_project_workflow = 1;
    centre->revision += 1U;
    if (out_workflow != NULL) *out_workflow = workflow;
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
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_developer_pipeline_centre_execute_batch_process(
    UmiStudioDeveloperPipelineCentre *centre,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_batch_execute_process(
        centre->runtime, request, out_snapshot);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiDeveloperRuntime *umi_studio_developer_pipeline_centre_runtime(
    UmiStudioDeveloperPipelineCentre *centre)
{
    return centre != NULL ? centre->runtime : NULL;
}

UmiBuildTaskRegistry *umi_studio_developer_pipeline_centre_delivery_tasks(
    UmiStudioDeveloperPipelineCentre *centre)
{
    return centre != NULL ? centre->delivery_tasks : NULL;
}

UmiBuildDeploymentTargetRegistry *
umi_studio_developer_pipeline_centre_deployment_targets(
    UmiStudioDeveloperPipelineCentre *centre)
{
    return centre != NULL ? centre->deployment_targets : NULL;
}

UmiBuildDeliveryPipeline *umi_studio_developer_pipeline_centre_delivery(
    UmiStudioDeveloperPipelineCentre *centre)
{
    return centre != NULL ? centre->delivery : NULL;
}

size_t umi_studio_build_delivery_command_contribution_count(void)
{
    return sizeof(DELIVERY_COMMANDS) / sizeof(DELIVERY_COMMANDS[0]);
}

const UmiStudioBuildDeliveryCommandContribution *
umi_studio_build_delivery_command_contribution_at(size_t position)
{
    if (position >= umi_studio_build_delivery_command_contribution_count()) {
        return NULL;
    }
    return &DELIVERY_COMMANDS[position];
}

const UmiStudioBuildDeliveryCommandContribution *
umi_studio_build_delivery_command_contribution_find(
    const char *framework_command_id)
{
    size_t index;
    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_build_delivery_command_contribution_count();
         ++index) {
        if (strcmp(DELIVERY_COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &DELIVERY_COMMANDS[index];
        }
    }
    return NULL;
}

size_t umi_studio_build_delivery_view_contribution_count(void)
{
    return sizeof(DELIVERY_VIEWS) / sizeof(DELIVERY_VIEWS[0]);
}

const UmiStudioBuildDeliveryViewContribution *
umi_studio_build_delivery_view_contribution_at(size_t position)
{
    if (position >= umi_studio_build_delivery_view_contribution_count()) {
        return NULL;
    }
    return &DELIVERY_VIEWS[position];
}

const UmiStudioBuildDeliveryViewContribution *
umi_studio_build_delivery_view_contribution_find(
    const char *framework_view_id)
{
    size_t index;
    if (framework_view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_build_delivery_view_contribution_count();
         ++index) {
        if (strcmp(DELIVERY_VIEWS[index].framework_view_id,
                   framework_view_id) == 0) {
            return &DELIVERY_VIEWS[index];
        }
    }
    return NULL;
}

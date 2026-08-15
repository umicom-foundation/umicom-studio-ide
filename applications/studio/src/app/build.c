/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/build.c
 *
 * PURPOSE:
 *   Implement Studio build profiles, execution, diagnostic capture and retained history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioBuildService {
    UmiBuildProfile profile;
    UmiBuildHistory *history;
    UmiBuildEngine *engine;
    UmiBuildArtifactIndex *artifacts;
    UmiBuildWorkspace *workspace;
    UmiCancellationToken *cancellation;
    UmiBuildResult last_result;
    int has_last_result;
};

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_build_service_create(const char *source_root,
                                           UmiClock *clock,
                                           UmiStudioBuildService **out_service)
{
    UmiStudioBuildService *service;
    UmiStatus status;

    if (source_root == NULL || source_root[0] == '\0' || clock == NULL ||
        out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiStudioBuildService *)calloc(1U, sizeof(*service));
    if (service == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_build_profile_init(&service->profile);
    status = umi_build_profile_set(&service->profile,
                                   "studio.development",
                                   source_root,
                                   "build/umicom-development");
    if (status == UMI_STATUS_OK) {
        status = copy_text(service->profile.generator,
                           sizeof(service->profile.generator),
                           "Ninja");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(service->profile.configuration,
                           sizeof(service->profile.configuration),
                           "Debug");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(service->profile.install_directory,
                           sizeof(service->profile.install_directory),
                           "build/umicom-development/install");
    }
    if (status == UMI_STATUS_OK) {
#ifdef _WIN32
        status = copy_text(service->profile.run_program,
                           sizeof(service->profile.run_program),
                           "build/umicom-development/bin/umicom-studio-ide.exe");
#else
        status = copy_text(service->profile.run_program,
                           sizeof(service->profile.run_program),
                           "build/umicom-development/bin/umicom-studio-ide");
#endif
    }
    service->profile.parallel_jobs = 4U;
    service->profile.timeout_ms = 0U;
    service->profile.build_testing = 1;
    service->profile.strict_warnings = 1;
    if (status == UMI_STATUS_OK) {
        status = umi_build_history_create(64U, &service->history);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_cancellation_token_create(&service->cancellation);
    }
    if (status == UMI_STATUS_OK) {
        UmiBuildEngineConfig engine_config;
        (void)memset(&engine_config, 0, sizeof(engine_config));
        engine_config.profile = service->profile;
        engine_config.history = service->history;
        engine_config.clock = clock;
        engine_config.cancellation = service->cancellation;
        status = umi_build_engine_create(&engine_config, &service->engine);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_build_artifact_index_create(&service->artifacts);
    }
    if (status == UMI_STATUS_OK) {
        UmiBuildWorkspaceBindings bindings;
        umi_build_workspace_bindings_init(&bindings);
        bindings.graph = umi_build_engine_graph(service->engine);
        bindings.history = service->history;
        bindings.artifacts = service->artifacts;
        bindings.profile = &service->profile;
        status = umi_build_workspace_create(&bindings, &service->workspace);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_build_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_build_service_destroy(UmiStudioBuildService *service)
{
    if (service == NULL) return;
    umi_build_workspace_destroy(service->workspace);
    umi_build_artifact_index_destroy(service->artifacts);
    umi_build_engine_destroy(service->engine);
    umi_cancellation_token_destroy(service->cancellation);
    umi_build_history_destroy(service->history);
    free(service);
}

UmiStatus umi_studio_build_service_bind_task_queue(
    UmiStudioBuildService *service,
    UmiTaskQueue *task_queue)
{
    if (service == NULL || task_queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_build_workspace_set_task_queue(service->workspace, task_queue);
}

UmiStatus umi_studio_build_service_set_profile(
    UmiStudioBuildService *service,
    const UmiBuildProfile *profile)
{
    UmiStatus status;
    char message[256];
    if (service == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_profile_validate(profile, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_engine_set_profile(service->engine, profile);
    if (status == UMI_STATUS_OK) service->profile = *profile;
    return status;
}

UmiStatus umi_studio_build_service_prepare_default_graph(
    UmiStudioBuildService *service,
    int include_run)
{
    UmiBuildGraph *graph;
    UmiBuildExecutionPolicy policy;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    graph = umi_build_engine_graph(service->engine);
    umi_build_execution_policy_init(&policy);
    policy.default_timeout_ms = service->profile.timeout_ms;
    policy.maximum_parallel_jobs = umi_build_policy_safe_parallel_jobs(
        service->profile.parallel_jobs, 0U, 0U);
    return umi_build_plan_populate_standard(graph, &policy, include_run);
}

UmiStatus umi_studio_build_service_execute_next(
    UmiStudioBuildService *service,
    UmiBuildResult *out_result)
{
    UmiStatus status;
    if (service == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_engine_execute_next(service->engine, out_result);
    if (status != UMI_STATUS_NOT_FOUND) {
        service->last_result = *out_result;
        service->has_last_result = 1;
        (void)umi_build_workspace_select_latest_operation(service->workspace);
    }
    return status;
}

UmiStatus umi_studio_build_service_execute_all(
    UmiStudioBuildService *service,
    size_t maximum_nodes,
    size_t *out_executed_count)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_engine_execute_all(service->engine, maximum_nodes,
                                          out_executed_count);
    if (umi_build_history_latest(service->history, &service->last_result) ==
        UMI_STATUS_OK) {
        service->has_last_result = 1;
        (void)umi_build_workspace_select_latest_operation(service->workspace);
    }
    return status;
}

void umi_studio_build_service_cancel(UmiStudioBuildService *service)
{
    if (service != NULL) umi_build_engine_request_cancel(service->engine);
}

UmiStatus umi_studio_build_service_retry(UmiStudioBuildService *service,
                                         const char *node_id)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_engine_retry(service->engine, node_id);
    if (status == UMI_STATUS_OK)
        (void)umi_build_workspace_select_node(service->workspace, node_id);
    return status;
}

UmiStatus umi_studio_build_service_invalidate(UmiStudioBuildService *service,
                                              const char *node_id,
                                              uint64_t input_revision)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_engine_invalidate(service->engine, node_id,
                                         input_revision);
    if (status == UMI_STATUS_OK)
        (void)umi_build_workspace_select_node(service->workspace, node_id);
    return status;
}

UmiBuildGraph *umi_studio_build_service_graph(UmiStudioBuildService *service)
{
    return service != NULL ? umi_build_engine_graph(service->engine) : NULL;
}

UmiBuildArtifactIndex *umi_studio_build_service_artifacts(
    UmiStudioBuildService *service)
{
    return service != NULL ? service->artifacts : NULL;
}

UmiStatus umi_studio_build_service_record_artifact(
    UmiStudioBuildService *service,
    const UmiBuildArtifactSnapshot *artifact)
{
    return service != NULL
        ? umi_build_artifact_index_upsert(service->artifacts, artifact)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_build_service_run(UmiStudioBuildService *service,
                                       UmiBuildPhase phase,
                                       UmiBuildResult *out_result)
{
    UmiStatus status;
    if (service == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_engine_execute_phase(service->engine, phase, out_result);
    service->last_result = *out_result;
    service->has_last_result = 1;
    (void)umi_build_workspace_select_latest_operation(service->workspace);
    return status;
}

UmiStatus umi_studio_build_service_snapshot(
    const UmiStudioBuildService *service,
    UmiStudioBuildSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)copy_text(out_snapshot->source_root,
                    sizeof(out_snapshot->source_root),
                    service->profile.source_directory);
    (void)copy_text(out_snapshot->build_directory,
                    sizeof(out_snapshot->build_directory),
                    service->profile.build_directory);
    (void)copy_text(out_snapshot->profile_id,
                    sizeof(out_snapshot->profile_id),
                    service->profile.profile_id);
    out_snapshot->history_count = umi_build_history_count(service->history);
    {
        UmiBuildEngineSnapshot engine_snapshot;
        if (umi_build_engine_snapshot(service->engine, &engine_snapshot) ==
            UMI_STATUS_OK)
            out_snapshot->next_operation_id = engine_snapshot.next_operation_id;
    }
    (void)umi_build_graph_snapshot(umi_build_engine_graph(service->engine),
                                   &out_snapshot->graph);
    out_snapshot->artifact_count =
        umi_build_artifact_index_count(service->artifacts);
    if (service->has_last_result) {
        out_snapshot->last_phase = service->last_result.phase;
        out_snapshot->last_state = service->last_result.state;
        out_snapshot->last_status = service->last_result.status;
        out_snapshot->last_exit_code = service->last_result.exit_code;
        out_snapshot->diagnostic_count =
            service->last_result.diagnostics.count;
    }
    return umi_build_workspace_snapshot(service->workspace,
                                        &out_snapshot->workspace);
}

UmiBuildHistory *umi_studio_build_service_history(UmiStudioBuildService *service)
{
    return service != NULL ? service->history : NULL;
}

const UmiBuildProfile *umi_studio_build_service_profile(
    const UmiStudioBuildService *service)
{
    return service != NULL ? &service->profile : NULL;
}

UmiBuildWorkspace *umi_studio_build_service_workspace(
    UmiStudioBuildService *service)
{
    return service != NULL ? service->workspace : NULL;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/build.h
 *
 * PURPOSE:
 *   Expose Studio build profiles, execution, diagnostics and history through Framework build services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_BUILD_H
#define UMICOM_STUDIO_BUILD_H

#include <stddef.h>

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioBuildService UmiStudioBuildService;

typedef struct UmiStudioBuildSnapshot {
    char source_root[UMI_BUILD_PATH_CAPACITY];
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    char profile_id[UMI_BUILD_ID_CAPACITY];
    size_t history_count;
    uint64_t next_operation_id;
    UmiBuildPhase last_phase;
    UmiBuildState last_state;
    UmiStatus last_status;
    int last_exit_code;
    size_t diagnostic_count;
    UmiBuildGraphSnapshot graph;
    size_t artifact_count;
    UmiBuildWorkspaceSnapshot workspace;
} UmiStudioBuildSnapshot;

UmiStatus umi_studio_build_service_create(const char *source_root,
                                           UmiClock *clock,
                                           UmiStudioBuildService **out_service);
void umi_studio_build_service_destroy(UmiStudioBuildService *service);
UmiStatus umi_studio_build_service_set_profile(
    UmiStudioBuildService *service,
    const UmiBuildProfile *profile
);
UmiStatus umi_studio_build_service_bind_task_queue(
    UmiStudioBuildService *service,
    UmiTaskQueue *task_queue
);
UmiStatus umi_studio_build_service_run(UmiStudioBuildService *service,
                                       UmiBuildPhase phase,
                                       UmiBuildResult *out_result);
UmiStatus umi_studio_build_service_prepare_default_graph(
    UmiStudioBuildService *service,
    int include_run);
UmiStatus umi_studio_build_service_execute_next(
    UmiStudioBuildService *service,
    UmiBuildResult *out_result);
UmiStatus umi_studio_build_service_execute_all(
    UmiStudioBuildService *service,
    size_t maximum_nodes,
    size_t *out_executed_count);
void umi_studio_build_service_cancel(UmiStudioBuildService *service);
UmiStatus umi_studio_build_service_retry(UmiStudioBuildService *service,
                                         const char *node_id);
UmiStatus umi_studio_build_service_invalidate(UmiStudioBuildService *service,
                                              const char *node_id,
                                              uint64_t input_revision);
UmiBuildGraph *umi_studio_build_service_graph(UmiStudioBuildService *service);
UmiBuildArtifactIndex *umi_studio_build_service_artifacts(
    UmiStudioBuildService *service);
UmiStatus umi_studio_build_service_record_artifact(
    UmiStudioBuildService *service,
    const UmiBuildArtifactSnapshot *artifact);
UmiStatus umi_studio_build_service_snapshot(
    const UmiStudioBuildService *service,
    UmiStudioBuildSnapshot *out_snapshot
);
UmiBuildHistory *umi_studio_build_service_history(
    UmiStudioBuildService *service
);
const UmiBuildProfile *umi_studio_build_service_profile(
    const UmiStudioBuildService *service
);
UmiBuildWorkspace *umi_studio_build_service_workspace(
    UmiStudioBuildService *service
);

#ifdef __cplusplus
}
#endif

#endif

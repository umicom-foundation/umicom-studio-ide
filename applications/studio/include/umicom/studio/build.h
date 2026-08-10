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
} UmiStudioBuildSnapshot;

UmiStatus umi_studio_build_service_create(const char *source_root,
                                           UmiClock *clock,
                                           UmiStudioBuildService **out_service);
void umi_studio_build_service_destroy(UmiStudioBuildService *service);
UmiStatus umi_studio_build_service_set_profile(
    UmiStudioBuildService *service,
    const UmiBuildProfile *profile
);
UmiStatus umi_studio_build_service_run(UmiStudioBuildService *service,
                                       UmiBuildPhase phase,
                                       UmiBuildResult *out_result);
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

#ifdef __cplusplus
}
#endif

#endif

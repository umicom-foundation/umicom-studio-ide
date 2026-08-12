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

#include "umicom/developer/developer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeveloperPipelineCentre UmiStudioDeveloperPipelineCentre;

typedef struct UmiStudioDeveloperPipelineCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char title[256];
    char summary[512];
    UmiDeveloperContextSnapshot context;
    UmiDeveloperPipelineSnapshot pipeline;
    UmiDeveloperJournalSnapshot journal;
    uint64_t revision;
    size_t item_count;
    int available;
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

UmiDeveloperRuntime *umi_studio_developer_pipeline_centre_runtime(
    UmiStudioDeveloperPipelineCentre *centre);

#ifdef __cplusplus
}
#endif

#endif

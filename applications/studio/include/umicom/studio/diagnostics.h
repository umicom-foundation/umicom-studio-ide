/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/diagnostics.h
 *
 * PURPOSE:
 *   Provide Studio-facing operations for emitting, summarising, reading, and
 *   clearing retained Framework diagnostics without exposing the diagnostic
 *   hub implementation to every Studio feature.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DIAGNOSTICS_H
#define UMICOM_STUDIO_DIAGNOSTICS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_diagnostics_emit(
    UmiStudioServices *services,
    UmiDiagnosticSeverity severity,
    const char *source,
    const char *message,
    uint64_t correlation_id
);

UmiStatus umi_studio_diagnostics_summary(
    UmiStudioServices *services,
    UmiDiagnosticStoreSummary *out_summary
);

size_t umi_studio_diagnostics_count(UmiStudioServices *services);

UmiStatus umi_studio_diagnostics_record_at(
    UmiStudioServices *services,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record
);

UmiStatus umi_studio_diagnostics_clear(UmiStudioServices *services);
UmiStatus umi_studio_diagnostics_clear_problems(UmiStudioServices *services);
UmiStatus umi_studio_diagnostics_clear_output(UmiStudioServices *services);
UmiStatus umi_studio_diagnostics_pipeline_snapshot(
    UmiStudioServices *services,
    UmiDiagnosticPipelineSnapshot *out_snapshot
);
UmiStatus umi_studio_diagnostics_ingest_build_result(
    UmiStudioServices *services,
    const UmiBuildResult *result
);

#ifdef __cplusplus
}
#endif

#endif

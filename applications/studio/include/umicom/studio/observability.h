/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/observability.h
 *
 * PURPOSE:
 *   Expose Studio operational evidence through Framework metrics, traces,
 *   audit, profiling, readiness and event-log services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_OBSERVABILITY_H
#define UMICOM_STUDIO_OBSERVABILITY_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioObservabilityReport {
    UmiObservabilitySnapshot snapshot;
    size_t successful_audits;
    size_t denied_audits;
    size_t failed_audits;
    size_t profile_samples;
} UmiStudioObservabilityReport;

UmiStatus umi_studio_observability_report(
    UmiStudioServices *services,
    UmiStudioObservabilityReport *out_report);
UmiStatus umi_studio_observability_event(UmiStudioServices *services,
                                         const char *category,
                                         const char *message,
                                         UmiDiagnosticSeverity severity,
                                         uint64_t correlation_id);
UmiStatus umi_studio_observability_begin(UmiStudioServices *services,
                                         const char *name,
                                         uint64_t trace_id,
                                         uint64_t parent_span_id,
                                         UmiProfileScope *out_scope,
                                         uint64_t *out_span_id);
UmiStatus umi_studio_observability_end(UmiStudioServices *services,
                                       const UmiProfileScope *scope,
                                       uint64_t span_id,
                                       UmiStatus operation_status);

#ifdef __cplusplus
}
#endif

#endif

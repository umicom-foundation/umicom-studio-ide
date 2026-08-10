/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/operations.h
 *
 * PURPOSE:
 *   Own Studio's shared Framework security, plug-in, observability and
 *   resilience services without duplicating their implementations in the
 *   product layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_OPERATIONS_H
#define UMICOM_STUDIO_OPERATIONS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioOperations UmiStudioOperations;

typedef struct UmiStudioOperationsReport {
    size_t identities;
    size_t roles;
    size_t sessions;
    size_t trusted_workspaces;
    size_t security_events;
    size_t denied_security_events;
    size_t plugins;
    size_t plugin_contributions;
    size_t metrics;
    size_t trace_spans;
    size_t audit_records;
    size_t readiness_checks;
    size_t operational_events;
    size_t supervised_components;
    int ready;
} UmiStudioOperationsReport;

UmiStatus umi_studio_operations_create(UmiClock *clock,
                                       UmiStudioOperations **out_operations);
void umi_studio_operations_destroy(UmiStudioOperations *operations);

UmiSecurityContext *umi_studio_operations_security(UmiStudioOperations *operations);
UmiPluginHost *umi_studio_operations_plugins(UmiStudioOperations *operations);
UmiMetricsRegistry *umi_studio_operations_metrics(UmiStudioOperations *operations);
UmiTraceStore *umi_studio_operations_traces(UmiStudioOperations *operations);
UmiAuditLog *umi_studio_operations_audit(UmiStudioOperations *operations);
UmiProfiler *umi_studio_operations_profiler(UmiStudioOperations *operations);
UmiReadinessRegistry *umi_studio_operations_readiness(UmiStudioOperations *operations);
UmiOperationalEventLog *umi_studio_operations_events(UmiStudioOperations *operations);
UmiResilienceSupervisor *umi_studio_operations_resilience(UmiStudioOperations *operations);
UmiHealthGate *umi_studio_operations_health_gate(UmiStudioOperations *operations);
UmiCircuitBreaker *umi_studio_operations_plugin_circuit(UmiStudioOperations *operations);
UmiRateLimiter *umi_studio_operations_tool_rate_limiter(UmiStudioOperations *operations);
uint64_t umi_studio_operations_now(UmiStudioOperations *operations);

UmiStatus umi_studio_operations_report(const UmiStudioOperations *operations,
                                       UmiStudioOperationsReport *out_report);

#ifdef __cplusplus
}
#endif

#endif

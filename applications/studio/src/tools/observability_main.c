/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/observability_main.c
 *
 * PURPOSE:
 *   Inspect Studio metrics, traces, audit, profiling and readiness evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/observability.h"

#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioObservabilityReport report;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);

    if (status == UMI_STATUS_OK) {
        status = umi_studio_observability_event(
            umi_studio_bootstrap_services(bootstrap),
            "studio.tool",
            "Observability report requested",
            UMI_DIAGNOSTIC_INFO,
            0U);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_observability_report(
            umi_studio_bootstrap_services(bootstrap),
            &report);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("Metrics: %zu\nTrace spans: %zu\nAudit records: %zu\n"
                     "Readiness checks: %zu\nOperational events: %zu\n"
                     "Profile samples: %zu\nReady: %s\n",
                     report.snapshot.metrics, report.snapshot.spans,
                     report.snapshot.audit_records,
                     report.snapshot.readiness_checks,
                     report.snapshot.operational_events,
                     report.profile_samples,
                     report.snapshot.ready ? "yes" : "no");
    } else {
        (void)fprintf(stderr, "Observability command failed: %s\n", umi_status_text(status));
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

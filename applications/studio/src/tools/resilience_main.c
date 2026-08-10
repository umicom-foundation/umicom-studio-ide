/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/resilience_main.c
 *
 * PURPOSE:
 *   Inspect Studio supervision, circuit-breaker and rate-limit state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/resilience.h"

#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioResilienceReport report;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);

    if (status == UMI_STATUS_OK) {
        status = umi_studio_resilience_report(
            umi_studio_bootstrap_services(bootstrap),
            &report);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("Supervised: %zu\nRunning: %zu\nDegraded: %zu\n"
                     "Failed: %zu\nPlugin circuit: %d\n"
                     "Tool tokens: %.2f\nTool accepted: %llu\nTool rejected: %llu\n",
                     report.supervised_components,
                     report.running_components,
                     report.degraded_components,
                     report.failed_components,
                     (int)report.plugin_circuit.state,
                     report.tool_rate_limit.tokens,
                     (unsigned long long)report.tool_rate_limit.accepted,
                     (unsigned long long)report.tool_rate_limit.rejected);
    } else {
        (void)fprintf(stderr, "Resilience command failed: %s\n", umi_status_text(status));
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

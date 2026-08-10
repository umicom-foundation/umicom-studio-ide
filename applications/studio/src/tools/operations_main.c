/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/operations_main.c
 *
 * PURPOSE:
 *   Display the combined Studio security, plug-in, observability and resilience summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/operations.h"

#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioOperationsReport report;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);

    if (status == UMI_STATUS_OK) {
        status = umi_studio_operations_report(
            umi_studio_services_operations(
                umi_studio_bootstrap_services(bootstrap)),
            &report);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("Identities: %zu\nRoles: %zu\nPlugins: %zu\n"
                     "Contributions: %zu\nMetrics: %zu\nTrace spans: %zu\n"
                     "Audit records: %zu\nReadiness checks: %zu\n"
                     "Operational events: %zu\nSupervised components: %zu\n"
                     "Ready: %s\n",
                     report.identities, report.roles, report.plugins,
                     report.plugin_contributions, report.metrics,
                     report.trace_spans, report.audit_records,
                     report.readiness_checks, report.operational_events,
                     report.supervised_components,
                     report.ready ? "yes" : "no");
    } else {
        (void)fprintf(stderr, "Operations command failed: %s\n", umi_status_text(status));
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

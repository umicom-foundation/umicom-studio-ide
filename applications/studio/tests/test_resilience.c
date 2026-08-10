/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_resilience.c
 *
 * PURPOSE:
 *   Verify Studio supervision reports, failure decisions and bounded tool rate
 *   limiting through Framework resilience services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/resilience.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioResilienceReport report;
    UmiRestartDecision decision;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_resilience_report(services, &report) == UMI_STATUS_OK);
    assert(report.supervised_components == 4U);
    assert(report.running_components == 4U);
    assert(umi_studio_resilience_failure(services,
                                         "studio.plugin-host",
                                         UMI_STATUS_UNAVAILABLE,
                                         &decision) == UMI_STATUS_OK);
    assert(decision.restart);
    assert(umi_studio_resilience_success(services,
                                         "studio.plugin-host") == UMI_STATUS_OK);
    assert(umi_studio_resilience_allow_tool(services, 1.0));
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

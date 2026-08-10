/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_operations.c
 *
 * PURPOSE:
 *   Verify construction and reporting of the combined Studio operational
 *   service container.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/operations.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioOperationsReport report;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_operations_report(
               umi_studio_services_operations(
                   umi_studio_bootstrap_services(bootstrap)),
               &report) == UMI_STATUS_OK);
    assert(report.identities == 1U);
    assert(report.roles == 1U);
    assert(report.metrics == 3U);
    assert(report.readiness_checks == 4U);
    assert(report.supervised_components == 4U);
    assert(report.ready);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

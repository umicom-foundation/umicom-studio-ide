/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_data.c
 *
 * PURPOSE:
 *   Verify Studio Data Server access, transactional updates and integrity reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/data.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioDataReport report;
    char value[128];
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_data_transaction_set(services, "studio.test", "value") == UMI_STATUS_OK);
    assert(umi_studio_data_get(services, "studio.test", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "value") == 0);
    assert(umi_studio_data_report(services, &report) == UMI_STATUS_OK);
    assert(report.records >= 1U);
    assert(umi_studio_data_remove(services, "studio.test") == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_history.c
 *
 * PURPOSE:
 *   Verify ordered Studio operational-history persistence and reconstruction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/history.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioHistoryRecord record;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_history_append(services, "build", UMI_STATUS_OK,
                                     "Build completed", 55U, NULL) == UMI_STATUS_OK);
    assert(umi_studio_history_count(services) == 1U);
    assert(umi_studio_history_read(services, 0U, &record) == UMI_STATUS_OK);
    assert(strcmp(record.category, "build") == 0);
    assert(strcmp(record.message, "Build completed") == 0);
    assert(record.correlation_id == 55U);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

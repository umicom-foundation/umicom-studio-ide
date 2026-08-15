/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_test_service.c
 *
 * PURPOSE:
 *   Verify Studio test-registry ownership before external CTest discovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/tests.h"

#include <assert.h>

int main(void)
{
    UmiStudioTestService *service = NULL;
    UmiStudioTestSnapshot snapshot;
    assert(umi_studio_test_service_create(&service) == UMI_STATUS_OK);
    assert(service != NULL);
    assert(umi_studio_test_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.suite_count == 1U);
    assert(snapshot.test_count == 0U);
    assert(snapshot.explorer_revision == 1U);
    assert(umi_studio_test_service_registry(service) != NULL);
    assert(umi_studio_test_service_platform(service) != NULL);
    assert(umi_studio_test_service_workspace(service) != NULL);
    umi_studio_test_service_destroy(service);
    return 0;
}

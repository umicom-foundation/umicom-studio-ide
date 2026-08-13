/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_test_execution_bridge.c
 *
 * PURPOSE:
 *   Verify Studio exposes the Framework execution contract and rejects an
 *   empty operation before attempting to invoke an external CTest process.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/tests.h"

int main(void)
{
    UmiStudioTestService *service = NULL;
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformExecutionSummary summary;
    assert(umi_studio_test_service_create(&service) == UMI_STATUS_OK);
    umi_test_platform_operation_plan_init(
        &plan, UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED);
    assert(umi_studio_test_service_execute(service, &plan, &summary) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_studio_test_service_platform(service) != NULL);
    umi_studio_test_service_destroy(service);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_debugger_service.c
 *
 * PURPOSE:
 *   Verify Studio Debug Adapter Protocol initialisation and breakpoint ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/debugger.h"

#include <assert.h>

int main(void)
{
    UmiStudioDebuggerService *service = NULL;
    UmiStudioDebuggerSnapshot snapshot;
    int64_t request_id = 0;
    assert(umi_studio_debugger_service_create(&service) == UMI_STATUS_OK);
    assert(umi_studio_debugger_service_initialize(service,
                                                   "umicom.debug",
                                                   &request_id) == UMI_STATUS_OK);
    assert(request_id == 1);
    assert(umi_studio_debugger_service_add_breakpoint(service,
                                                       "main.c",
                                                       12,
                                                       1) == UMI_STATUS_OK);
    assert(umi_studio_debugger_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.initialized);
    assert(snapshot.breakpoint_count == 1U);
    assert(snapshot.queued_messages == 1U);
    umi_studio_debugger_service_destroy(service);
    return 0;
}

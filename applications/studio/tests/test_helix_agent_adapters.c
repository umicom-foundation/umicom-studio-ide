/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_adapters.c
 * PURPOSE: Verify Studio can block source-control actions after approval.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/helix_agent_adapters.h"
#include "umicom/studio/helix_agent_operations.h"
int main(void)
{
    UmiStudioHelixAgentAdapterState state;
    UmiHelixExecutionAdapter adapter;
    UmiHelixAction action;
    char evidence[256];
    umi_studio_helix_agent_adapters_init(&state);
    assert(umi_studio_helix_agent_adapters_create(&state, &adapter)
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_source_control_action("main", &action)
           == UMI_STATUS_OK);
    assert(umi_helix_execution_adapter_execute(
        &adapter, &action, 1, evidence, sizeof(evidence))
        == UMI_STATUS_PERMISSION_DENIED);
    assert(strstr(evidence, "blocked") != NULL && state.execution_count == 0U);
    return 0;
}

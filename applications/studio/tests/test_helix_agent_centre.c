/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_centre.c
 * PURPOSE: Verify Studio composes the Framework Helix v2 runtime and adapters.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/helix_agent_centre.h"
int main(void)
{
    UmiStudioHelixAgentCentre *centre = NULL;
    UmiStudioHelixAgentCentreConfig config =
        umi_studio_helix_agent_centre_config_default();
    UmiHelixOrchestrator *runtime;
    assert(umi_studio_helix_agent_centre_create(&config, &centre) == UMI_STATUS_OK);
    runtime = umi_studio_helix_agent_centre_runtime(centre);
    assert(runtime != NULL && runtime->agents.count == 5U);
    assert(runtime->adapter_configured);
    umi_studio_helix_agent_centre_destroy(centre);
    return 0;
}

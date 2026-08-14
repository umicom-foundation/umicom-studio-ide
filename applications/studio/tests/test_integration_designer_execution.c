/* Umicom Studio IDE Tests | Deterministic execution | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_seeded_integration_designer();
    UmiIntegrationSimulation simulation;
    assert(umi_studio_integration_designer_simulate(centre,"order-sync",&simulation) == UMI_STATUS_OK);
    assert(simulation.status == UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED && simulation.trace_count == 3U);
    assert(strcmp(simulation.trace[1].detail,"Request planned without network I/O") == 0);
    assert(centre->fabric.runs.count == 1U);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

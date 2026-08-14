/* Umicom Studio IDE Tests | Execution monitoring | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
#include "umicom/studio/integration_designer_monitoring.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_seeded_integration_designer();
    UmiIntegrationSimulation simulation;
    UmiStudioIntegrationDesignerHealth health;
    assert(umi_studio_integration_designer_simulate(centre,"order-sync",&simulation) == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_health(centre,&health) == UMI_STATUS_OK);
    assert(health.healthy && health.enabled_connections == 1U && health.runs.succeeded == 1U);
    assert(umi_studio_integration_designer_find_run(centre,simulation.run_id) != NULL);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

/* Umicom Studio IDE | Native Integration Designer command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/integration_designer_commands.h"
#include "umicom/studio/integration_designer_monitoring.h"
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioIntegrationDesignerCentre *centre = NULL;
    UmiIntegrationSimulation simulation;
    UmiStudioIntegrationDesignerHealth health;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_integration_designer_create(umi_studio_bootstrap_services(bootstrap),&centre);
    if (status == UMI_STATUS_OK) status = umi_studio_integration_designer_execute(centre,UMI_STUDIO_INTEGRATION_COMMAND_SEED_SAMPLE,NULL);
    if (status == UMI_STATUS_OK) status = umi_studio_integration_designer_execute(centre,UMI_STUDIO_INTEGRATION_COMMAND_SIMULATE,&simulation);
    if (status == UMI_STATUS_OK) status = umi_studio_integration_designer_health(centre,&health);
    if (status == UMI_STATUS_OK) (void)printf("Integration Designer: %zu connection(s), %zu successful run(s), %zu step(s)\n",health.enabled_connections,health.runs.succeeded,simulation.trace_count);
    umi_studio_integration_designer_destroy(centre);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/* Umicom Studio IDE | Integration Designer monitoring | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_monitoring.h"
#include <string.h>

UmiStatus umi_studio_integration_designer_health(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerHealth *out_health)
{
    size_t index;
    if (centre == NULL || out_health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_health,0,sizeof(*out_health));
    umi_integration_run_history_metrics(&centre->fabric.runs,&out_health->runs);
    for (index = 0U; index < centre->fabric.connections.count; ++index) {
        if (centre->fabric.connections.items[index].enabled) out_health->enabled_connections += 1U;
        else out_health->disabled_connections += 1U;
    }
    out_health->healthy = out_health->disabled_connections == 0U && out_health->runs.failed == 0U;
    return UMI_STATUS_OK;
}

const UmiIntegrationRunRecord *umi_studio_integration_designer_find_run(const UmiStudioIntegrationDesignerCentre *centre,const char *run_id)
{
    return centre == NULL ? NULL : umi_integration_run_history_find(&centre->fabric.runs,run_id);
}

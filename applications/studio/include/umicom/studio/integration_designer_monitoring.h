/* Umicom Studio IDE | Integration Designer monitoring | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_MONITORING_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_MONITORING_H
#include "umicom/studio/integration_designer_centre.h"
typedef struct UmiStudioIntegrationDesignerHealth {
    UmiIntegrationRunMetrics runs;
    size_t enabled_connections;
    size_t disabled_connections;
    bool healthy;
} UmiStudioIntegrationDesignerHealth;
UmiStatus umi_studio_integration_designer_health(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerHealth *out_health);
const UmiIntegrationRunRecord *umi_studio_integration_designer_find_run(const UmiStudioIntegrationDesignerCentre *centre,const char *run_id);
#endif

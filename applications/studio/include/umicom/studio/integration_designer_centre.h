/* Umicom Studio IDE | Integration Designer centre | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_CENTRE_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_CENTRE_H
#include "umicom/studio/services.h"
#include "umicom/integration/designer.h"
typedef struct UmiStudioIntegrationDesignerCentre {
    UmiStudioServices *services;
    UmiIntegrationDesignerFabric fabric;
    UmiIntegrationExecutionPolicy policy;
    char active_view[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    uint64_t next_run_sequence;
    uint64_t revision;
} UmiStudioIntegrationDesignerCentre;
typedef struct UmiStudioIntegrationDesignerSnapshot {
    UmiIntegrationDesignerSnapshot fabric;
    char active_view[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiStudioIntegrationDesignerSnapshot;
UmiStatus umi_studio_integration_designer_create(UmiStudioServices *services,UmiStudioIntegrationDesignerCentre **out_centre);
void umi_studio_integration_designer_destroy(UmiStudioIntegrationDesignerCentre *centre);
UmiStatus umi_studio_integration_designer_activate(UmiStudioIntegrationDesignerCentre *centre,const char *view_id);
UmiStatus umi_studio_integration_designer_snapshot(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerSnapshot *out_snapshot);
UmiIntegrationDesignerFabric *umi_studio_integration_designer_fabric(UmiStudioIntegrationDesignerCentre *centre);
#endif

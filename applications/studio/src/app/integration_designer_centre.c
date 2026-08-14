/* Umicom Studio IDE | Integration Designer centre | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_centre.h"
#include <stdlib.h>
#include <string.h>

UmiStatus umi_studio_integration_designer_create(UmiStudioServices *services,UmiStudioIntegrationDesignerCentre **out_centre)
{
    UmiStudioIntegrationDesignerCentre *centre;
    UmiStatus status;
    if (services == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioIntegrationDesignerCentre *)calloc(1U,sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->services = services;
    umi_integration_designer_fabric_init(&centre->fabric);
    umi_integration_execution_policy_default(&centre->policy);
    status = umi_integration_designer_copy(centre->active_view,sizeof(centre->active_view),"overview");
    if (status != UMI_STATUS_OK) { free(centre); return status; }
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_integration_designer_destroy(UmiStudioIntegrationDesignerCentre *centre)
{
    free(centre);
}

UmiStatus umi_studio_integration_designer_activate(UmiStudioIntegrationDesignerCentre *centre,const char *view_id)
{
    UmiStatus status;
    if (centre == NULL || view_id == NULL || view_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_designer_copy(centre->active_view,sizeof(centre->active_view),view_id);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_integration_designer_snapshot(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerSnapshot *out_snapshot)
{
    UmiStatus status;
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    umi_integration_designer_fabric_snapshot(&centre->fabric,&out_snapshot->fabric);
    status = umi_integration_designer_copy(out_snapshot->active_view,sizeof(out_snapshot->active_view),centre->active_view);
    out_snapshot->revision = centre->revision;
    return status;
}

UmiIntegrationDesignerFabric *umi_studio_integration_designer_fabric(UmiStudioIntegrationDesignerCentre *centre)
{
    return centre == NULL ? NULL : &centre->fabric;
}

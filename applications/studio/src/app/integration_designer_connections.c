/* Umicom Studio IDE | Integration Designer connections | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_connections.h"

UmiStatus umi_studio_integration_designer_add_connection(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationConnectionProfile *profile)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_connection_catalogue_add(&centre->fabric.connections,profile);
    if (status == UMI_STATUS_OK) { centre->fabric.revision += 1U; centre->revision += 1U; }
    return status;
}

UmiStatus umi_studio_integration_designer_add_schema(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationSchemaDescriptor *schema)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_schema_catalogue_register(&centre->fabric.schemas,schema);
    if (status == UMI_STATUS_OK) { centre->fabric.revision += 1U; centre->revision += 1U; }
    return status;
}

const UmiIntegrationConnectionProfile *umi_studio_integration_designer_find_connection(const UmiStudioIntegrationDesignerCentre *centre,const char *profile_id)
{
    return centre == NULL ? NULL : umi_integration_connection_catalogue_find(&centre->fabric.connections,profile_id);
}

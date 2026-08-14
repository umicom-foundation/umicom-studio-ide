/* Umicom Studio IDE | Integration Designer connections | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_CONNECTIONS_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_CONNECTIONS_H
#include "umicom/studio/integration_designer_centre.h"
UmiStatus umi_studio_integration_designer_add_connection(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationConnectionProfile *profile);
UmiStatus umi_studio_integration_designer_add_schema(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationSchemaDescriptor *schema);
const UmiIntegrationConnectionProfile *umi_studio_integration_designer_find_connection(const UmiStudioIntegrationDesignerCentre *centre,const char *profile_id);
#endif

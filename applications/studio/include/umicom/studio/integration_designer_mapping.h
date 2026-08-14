/* Umicom Studio IDE | Integration Designer mapping | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_MAPPING_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_MAPPING_H
#include "umicom/studio/integration_designer_centre.h"
UmiStatus umi_studio_integration_designer_add_mapping(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationFieldMapping *mapping);
const UmiIntegrationFieldMapping *umi_studio_integration_designer_find_mapping(const UmiStudioIntegrationDesignerCentre *centre,const char *mapping_id);
UmiStatus umi_studio_integration_designer_preview_mapping(const UmiStudioIntegrationDesignerCentre *centre,const char *mapping_id,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation);
#endif

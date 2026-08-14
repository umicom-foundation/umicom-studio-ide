/* Umicom Studio IDE | Integration Designer API explorer | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_API_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_API_H
#include "umicom/studio/integration_designer_centre.h"
UmiStatus umi_studio_integration_designer_add_contract(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationApiContract *contract);
const UmiIntegrationApiContract *umi_studio_integration_designer_find_contract(const UmiStudioIntegrationDesignerCentre *centre,const char *contract_id);
UmiStatus umi_studio_integration_designer_plan_request(const UmiStudioIntegrationDesignerCentre *centre,const char *contract_id,const char *operation_id,const char *payload,const char *correlation_id,UmiIntegrationRequestPlan *out_plan);
#endif

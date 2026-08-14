/* Umicom Studio IDE | Integration Designer workflow canvas | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_WORKFLOW_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_WORKFLOW_H
#include "umicom/studio/integration_designer_centre.h"
UmiStatus umi_studio_integration_designer_add_workflow(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationWorkflow *workflow);
UmiStatus umi_studio_integration_designer_validate_workflow(const UmiStudioIntegrationDesignerCentre *centre,const char *workflow_id,UmiIntegrationDesignerValidation *validation);
#endif

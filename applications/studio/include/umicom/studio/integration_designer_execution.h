/* Umicom Studio IDE | Integration Designer execution | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_EXECUTION_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_EXECUTION_H
#include "umicom/studio/integration_designer_centre.h"
UmiStatus umi_studio_integration_designer_set_policy(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationExecutionPolicy *policy);
UmiStatus umi_studio_integration_designer_simulate(UmiStudioIntegrationDesignerCentre *centre,const char *workflow_id,UmiIntegrationSimulation *out_simulation);
#endif

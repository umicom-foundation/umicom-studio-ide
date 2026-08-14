/* Umicom Studio IDE | Integration Designer commands | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_COMMANDS_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_COMMANDS_H
#include "umicom/studio/integration_designer_execution.h"
typedef enum UmiStudioIntegrationDesignerCommand {
    UMI_STUDIO_INTEGRATION_COMMAND_SEED_SAMPLE = 1,
    UMI_STUDIO_INTEGRATION_COMMAND_VALIDATE,
    UMI_STUDIO_INTEGRATION_COMMAND_SIMULATE
} UmiStudioIntegrationDesignerCommand;
UmiStatus umi_studio_integration_designer_seed_sample(UmiStudioIntegrationDesignerCentre *centre);
UmiStatus umi_studio_integration_designer_execute(UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerCommand command,UmiIntegrationSimulation *out_simulation);
#endif

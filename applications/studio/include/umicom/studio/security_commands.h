/* Umicom Studio IDE | Security Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_COMMANDS_H
#define UMICOM_STUDIO_SECURITY_COMMANDS_H
#include "umicom/studio/security_roles.h"
typedef enum UmiStudioSecurityCommand { UMI_STUDIO_SECURITY_COMMAND_SEED = 1, UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_BUILD, UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_PLUGIN } UmiStudioSecurityCommand;
UmiStatus umi_studio_security_seed(UmiStudioSecurityCentre *centre);
UmiStatus umi_studio_security_execute(UmiStudioSecurityCentre *centre,UmiStudioSecurityCommand command,UmiSecurityGovernanceDecision *out_decision);
#endif

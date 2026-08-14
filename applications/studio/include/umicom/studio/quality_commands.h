/* Umicom Studio IDE | Quality Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_COMMANDS_H
#define UMICOM_STUDIO_QUALITY_COMMANDS_H
#include "umicom/studio/quality_gates.h"
typedef enum UmiStudioQualityCommand {
    UMI_STUDIO_QUALITY_COMMAND_SEED_EVIDENCE = 1,
    UMI_STUDIO_QUALITY_COMMAND_EVALUATE,
    UMI_STUDIO_QUALITY_COMMAND_REFRESH_REMEDIATION
} UmiStudioQualityCommand;
UmiStatus umi_studio_quality_seed_evidence(UmiStudioQualityCentre *centre);
UmiStatus umi_studio_quality_execute(UmiStudioQualityCentre *centre,UmiStudioQualityCommand command,UmiCodeGuardQualityEvaluation *out_evaluation);
#endif

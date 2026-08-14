/* Umicom Studio IDE | Operations commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_OPERATIONS_COMMANDS_H
#define UMICOM_STUDIO_OPERATIONS_COMMANDS_H
#include "umicom/studio/operations_centre.h"
typedef enum UmiStudioOperationsCommand {
    UMI_STUDIO_OPERATIONS_COMMAND_SEED = 1,
    UMI_STUDIO_OPERATIONS_COMMAND_MARK_CRASH_RECOVERED,
    UMI_STUDIO_OPERATIONS_COMMAND_OPEN_PROFILER
} UmiStudioOperationsCommand;
UmiStatus umi_studio_operations_seed(UmiStudioOperationsCentre *centre);
UmiStatus umi_studio_operations_execute(UmiStudioOperationsCentre *centre,UmiStudioOperationsCommand command,const char *argument);
#endif

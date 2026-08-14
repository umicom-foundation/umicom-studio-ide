/* Umicom Studio IDE | Operations commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_OPERATIONS_COMMANDS_V2_H
#define UMICOM_STUDIO_OPERATIONS_COMMANDS_V2_H
#include "umicom/studio/operations_centre_v2.h"
typedef enum UmiStudioOperationsCommandV2 {
    UMI_STUDIO_OPERATIONS_COMMAND_SEED = 1,
    UMI_STUDIO_OPERATIONS_COMMAND_MARK_CRASH_RECOVERED,
    UMI_STUDIO_OPERATIONS_COMMAND_OPEN_PROFILER
} UmiStudioOperationsCommandV2;
UmiStatus umi_studio_operations_seed_v2(UmiStudioOperationsCentreV2 *centre);
UmiStatus umi_studio_operations_execute_v2(UmiStudioOperationsCentreV2 *centre,UmiStudioOperationsCommandV2 command,const char *argument);
#endif

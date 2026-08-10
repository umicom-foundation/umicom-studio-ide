/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/terminal.h
 *
 * PURPOSE:
 *   Expose prepared terminal sessions and captured transcripts to Studio frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_TERMINAL_H
#define UMICOM_STUDIO_TERMINAL_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioTerminalService UmiStudioTerminalService;

typedef struct UmiStudioTerminalSnapshot {
    size_t session_count;
    UmiTerminalSessionSnapshot primary;
} UmiStudioTerminalSnapshot;

UmiStatus umi_studio_terminal_service_create(
    const char *working_directory,
    UmiClock *clock,
    UmiStudioTerminalService **out_service
);
void umi_studio_terminal_service_destroy(UmiStudioTerminalService *service);
UmiStatus umi_studio_terminal_service_execute(
    UmiStudioTerminalService *service,
    const char *command,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code
);
UmiStatus umi_studio_terminal_service_snapshot(
    const UmiStudioTerminalService *service,
    UmiStudioTerminalSnapshot *out_snapshot
);
UmiTerminalManager *umi_studio_terminal_service_manager(
    UmiStudioTerminalService *service
);
UmiTerminalSession *umi_studio_terminal_service_primary(
    UmiStudioTerminalService *service
);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/terminal.c
 *
 * PURPOSE:
 *   Implement Studio terminal-session ownership and command execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/terminal.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioTerminalService {
    UmiTerminalManager *manager;
    UmiTerminalSession *primary;
};

UmiStatus umi_studio_terminal_service_create(
    const char *working_directory,
    UmiClock *clock,
    UmiStudioTerminalService **out_service)
{
    UmiStudioTerminalService *service;
    UmiStatus status;
    if (working_directory == NULL || clock == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiStudioTerminalService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_terminal_manager_create(clock, &service->manager);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_manager_open(service->manager,
                                           "studio.primary",
                                           "Umicom Studio",
                                           working_directory,
                                           &service->primary);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_terminal_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_terminal_service_destroy(UmiStudioTerminalService *service)
{
    if (service == NULL) return;
    umi_terminal_manager_destroy(service->manager);
    free(service);
}

UmiStatus umi_studio_terminal_service_execute(
    UmiStudioTerminalService *service,
    const char *command,
    uint32_t timeout_ms,
    UmiCancellationToken *cancellation,
    int *out_exit_code)
{
    if (service == NULL || command == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_terminal_session_execute(service->primary,
                                        command,
                                        timeout_ms,
                                        cancellation,
                                        out_exit_code);
}

UmiStatus umi_studio_terminal_service_snapshot(
    const UmiStudioTerminalService *service,
    UmiStudioTerminalSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->session_count = umi_terminal_manager_count(service->manager);
    return umi_terminal_session_snapshot(service->primary,
                                         &out_snapshot->primary);
}

UmiTerminalManager *umi_studio_terminal_service_manager(
    UmiStudioTerminalService *service)
{
    return service != NULL ? service->manager : NULL;
}

UmiTerminalSession *umi_studio_terminal_service_primary(
    UmiStudioTerminalService *service)
{
    return service != NULL ? service->primary : NULL;
}

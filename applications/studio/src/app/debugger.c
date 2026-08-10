/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/debugger.c
 *
 * PURPOSE:
 *   Implement Studio debug-adapter protocol state and source breakpoint ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/debugger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioDebuggerService {
    UmiProtocolTransport *transport;
    UmiProtocolClient *client;
    UmiDapClient dap;
    UmiDapBreakpointRegistry *breakpoints;
};

UmiStatus umi_studio_debugger_service_create(
    UmiStudioDebuggerService **out_service)
{
    UmiStudioDebuggerService *service;
    UmiStatus status;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiStudioDebuggerService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_protocol_transport_create_memory(256U, &service->transport);
    if (status == UMI_STATUS_OK) {
        status = umi_protocol_client_create(service->transport,
                                            &service->client);
    }
    if (status == UMI_STATUS_OK) status = umi_protocol_client_start(service->client);
    if (status == UMI_STATUS_OK) status = umi_dap_client_init(&service->dap, service->client);
    if (status == UMI_STATUS_OK) {
        status = umi_dap_breakpoint_registry_create(&service->breakpoints);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_debugger_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_debugger_service_destroy(UmiStudioDebuggerService *service)
{
    if (service == NULL) return;
    umi_dap_breakpoint_registry_destroy(service->breakpoints);
    umi_protocol_client_destroy(service->client);
    umi_protocol_transport_destroy(service->transport);
    free(service);
}

UmiStatus umi_studio_debugger_service_initialize(
    UmiStudioDebuggerService *service,
    const char *adapter_id,
    int64_t *out_request_id)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_dap_initialize(&service->dap, adapter_id, out_request_id);
}

UmiStatus umi_studio_debugger_service_launch(
    UmiStudioDebuggerService *service,
    const char *program,
    const char *working_directory,
    int64_t *out_request_id)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_dap_launch(&service->dap,
                          program,
                          working_directory,
                          out_request_id);
}

UmiStatus umi_studio_debugger_service_add_breakpoint(
    UmiStudioDebuggerService *service,
    const char *source_path,
    int line,
    int column)
{
    UmiDapBreakpoint breakpoint;
    size_t length;
    if (service == NULL || source_path == NULL || line <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source_path);
    if (length + 1U > sizeof(breakpoint.source_path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&breakpoint, 0, sizeof(breakpoint));
    (void)memcpy(breakpoint.source_path, source_path, length + 1U);
    breakpoint.line = line;
    breakpoint.column = column;
    breakpoint.enabled = 1;
    return umi_dap_breakpoint_add(service->breakpoints, &breakpoint);
}

UmiStatus umi_studio_debugger_service_snapshot(
    const UmiStudioDebuggerService *service,
    UmiStudioDebuggerSnapshot *out_snapshot)
{
    UmiProtocolTransportStats stats;
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    stats = umi_protocol_transport_stats(service->transport);
    out_snapshot->client_state = umi_protocol_client_state(service->client);
    out_snapshot->initialized = service->dap.initialized;
    out_snapshot->breakpoint_count =
        umi_dap_breakpoint_count(service->breakpoints);
    out_snapshot->queued_messages = stats.queued;
    out_snapshot->sent_messages = stats.sent;
    out_snapshot->received_messages = stats.received;
    return UMI_STATUS_OK;
}

UmiProtocolTransport *umi_studio_debugger_service_transport(
    UmiStudioDebuggerService *service)
{
    return service != NULL ? service->transport : NULL;
}

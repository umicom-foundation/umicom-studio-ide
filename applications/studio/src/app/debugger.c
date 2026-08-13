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
    UmiDebugService *model;
    UmiDebugController *controller;
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
    if (status == UMI_STATUS_OK) status = umi_debug_service_create(&service->model);
    if (status == UMI_STATUS_OK) {
        status = umi_debug_controller_create(&service->dap, service->model,
                                             &service->controller);
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
    umi_debug_controller_destroy(service->controller);
    umi_debug_service_destroy(service->model);
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
    {
        UmiStatus status = umi_debug_controller_initialize(
            service->controller, adapter_id);
        if (status == UMI_STATUS_OK && out_request_id != NULL) {
            UmiDebugControllerSnapshot snapshot;
            if (umi_debug_controller_snapshot(service->controller,
                                               &snapshot) == UMI_STATUS_OK) {
                *out_request_id = snapshot.last_request_id;
            }
        }
        return status;
    }
}

UmiStatus umi_studio_debugger_service_launch(
    UmiStudioDebuggerService *service,
    const char *program,
    const char *working_directory,
    int64_t *out_request_id)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    {
        UmiStatus status = umi_debug_controller_launch(
            service->controller, program, working_directory);
        if (status == UMI_STATUS_OK && out_request_id != NULL) {
            UmiDebugControllerSnapshot snapshot;
            if (umi_debug_controller_snapshot(service->controller,
                                               &snapshot) == UMI_STATUS_OK) {
                *out_request_id = snapshot.last_request_id;
            }
        }
        return status;
    }
}

UmiStatus umi_studio_debugger_service_start(UmiStudioDebuggerService *service,
    const char *adapter_id, const char *program, const char *working_directory)
{
    UmiDebugControllerSnapshot snapshot;
    UmiStatus status;
    if (service == NULL || adapter_id == NULL || program == NULL ||
        working_directory == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_controller_snapshot(service->controller, &snapshot);
    if (status == UMI_STATUS_OK &&
        (snapshot.state == UMI_DEBUG_CONTROLLER_IDLE ||
         snapshot.state == UMI_DEBUG_CONTROLLER_TERMINATED)) {
        status = umi_debug_controller_initialize(service->controller,
                                                 adapter_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_debug_controller_launch(service->controller, program,
                                             working_directory);
    }
    return status;
}

UmiStatus umi_studio_debugger_service_continue(UmiStudioDebuggerService *service,int thread_id){return service!=NULL?umi_debug_controller_continue(service->controller,thread_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_debugger_service_pause(UmiStudioDebuggerService *service,int thread_id){return service!=NULL?umi_debug_controller_pause(service->controller,thread_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_debugger_service_next(UmiStudioDebuggerService *service,int thread_id){return service!=NULL?umi_debug_controller_next(service->controller,thread_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_debugger_service_step_in(UmiStudioDebuggerService *service,int thread_id){return service!=NULL?umi_debug_controller_step_in(service->controller,thread_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_debugger_service_step_out(UmiStudioDebuggerService *service,int thread_id){return service!=NULL?umi_debug_controller_step_out(service->controller,thread_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_debugger_service_stop(UmiStudioDebuggerService *service,int restart){return service!=NULL?umi_debug_controller_terminate(service->controller,restart):UMI_STATUS_INVALID_ARGUMENT;}

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
    {
        UmiStatus status = umi_dap_breakpoint_add(service->breakpoints,
                                                  &breakpoint);
        if (status == UMI_STATUS_OK) {
            UmiDebugBreakpointSnapshot model_breakpoint = {0};
            (void)snprintf(model_breakpoint.id, sizeof(model_breakpoint.id),
                           "%s:%d:%d", source_path, line, column);
            (void)snprintf(model_breakpoint.uri,
                           sizeof(model_breakpoint.uri), "%s", source_path);
            model_breakpoint.line = (uint32_t)line;
            model_breakpoint.column = column > 0 ? (uint32_t)column : 0U;
            model_breakpoint.enabled = 1;
            status = umi_debug_breakpoint_registry_upsert(
                umi_debug_service_breakpoint(service->model),
                &model_breakpoint);
        }
        return status;
    }
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
    {
        UmiDebugServiceSnapshot model;
        UmiDebugControllerSnapshot controller;
        if (umi_debug_service_snapshot(service->model, &model) == UMI_STATUS_OK) {
            out_snapshot->session_count = model.session_count;
            out_snapshot->thread_count = model.thread_count;
            out_snapshot->stack_frame_count = model.stack_frame_count;
            out_snapshot->variable_count = model.variable_count;
            out_snapshot->watch_count = model.watch_count;
            out_snapshot->event_count = model.event_count;
        }
        if (umi_debug_controller_snapshot(service->controller,
                                          &controller) == UMI_STATUS_OK) {
            (void)snprintf(out_snapshot->controller_state,
                           sizeof(out_snapshot->controller_state), "%s",
                           umi_debug_controller_state_text(controller.state));
        }
    }
    return UMI_STATUS_OK;
}

UmiDebugService *umi_studio_debugger_service_model(
    UmiStudioDebuggerService *service)
{
    return service != NULL ? service->model : NULL;
}

UmiDebugController *umi_studio_debugger_service_controller(
    UmiStudioDebuggerService *service)
{
    return service != NULL ? service->controller : NULL;
}

UmiProtocolTransport *umi_studio_debugger_service_transport(
    UmiStudioDebuggerService *service)
{
    return service != NULL ? service->transport : NULL;
}

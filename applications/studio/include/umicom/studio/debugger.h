/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/debugger.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral Studio debugger service over Debug Adapter Protocol contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_DEBUGGER_H
#define UMICOM_STUDIO_DEBUGGER_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDebuggerService UmiStudioDebuggerService;

typedef struct UmiStudioDebuggerSnapshot {
    UmiProtocolClientState client_state;
    int initialized;
    size_t breakpoint_count;
    size_t queued_messages;
    size_t sent_messages;
    size_t received_messages;
    size_t session_count;
    size_t thread_count;
    size_t stack_frame_count;
    size_t variable_count;
    size_t watch_count;
    size_t event_count;
    char controller_state[64];
    UmiDebugWorkspaceSnapshot workspace;
} UmiStudioDebuggerSnapshot;

UmiStatus umi_studio_debugger_service_create(
    UmiStudioDebuggerService **out_service
);
void umi_studio_debugger_service_destroy(UmiStudioDebuggerService *service);
UmiStatus umi_studio_debugger_service_initialize(
    UmiStudioDebuggerService *service,
    const char *adapter_id,
    int64_t *out_request_id
);
UmiStatus umi_studio_debugger_service_launch(
    UmiStudioDebuggerService *service,
    const char *program,
    const char *working_directory,
    int64_t *out_request_id
);
UmiStatus umi_studio_debugger_service_start(
    UmiStudioDebuggerService *service, const char *adapter_id,
    const char *program, const char *working_directory
);
UmiStatus umi_studio_debugger_service_continue(
    UmiStudioDebuggerService *service, int thread_id
);
UmiStatus umi_studio_debugger_service_pause(
    UmiStudioDebuggerService *service, int thread_id
);
UmiStatus umi_studio_debugger_service_next(
    UmiStudioDebuggerService *service, int thread_id
);
UmiStatus umi_studio_debugger_service_step_in(
    UmiStudioDebuggerService *service, int thread_id
);
UmiStatus umi_studio_debugger_service_step_out(
    UmiStudioDebuggerService *service, int thread_id
);
UmiStatus umi_studio_debugger_service_stop(
    UmiStudioDebuggerService *service, int restart
);
UmiStatus umi_studio_debugger_service_add_breakpoint(
    UmiStudioDebuggerService *service,
    const char *source_path,
    int line,
    int column
);
UmiStatus umi_studio_debugger_service_set_breakpoint_enabled(
    UmiStudioDebuggerService *service,
    const char *breakpoint_id,
    int enabled
);
UmiStatus umi_studio_debugger_service_remove_breakpoint(
    UmiStudioDebuggerService *service,
    const char *breakpoint_id
);
UmiStatus umi_studio_debugger_service_add_watch(
    UmiStudioDebuggerService *service,
    const char *expression,
    char *out_watch_id,
    size_t out_watch_id_capacity
);
UmiStatus umi_studio_debugger_service_remove_watch(
    UmiStudioDebuggerService *service,
    const char *watch_id
);
UmiStatus umi_studio_debugger_service_select_thread(
    UmiStudioDebuggerService *service,
    const char *thread_id
);
UmiStatus umi_studio_debugger_service_select_frame(
    UmiStudioDebuggerService *service,
    const char *frame_id
);
UmiStatus umi_studio_debugger_service_select_scope(
    UmiStudioDebuggerService *service,
    const char *scope_id
);
UmiStatus umi_studio_debugger_service_clear_console(
    UmiStudioDebuggerService *service
);
UmiStatus umi_studio_debugger_service_snapshot(
    const UmiStudioDebuggerService *service,
    UmiStudioDebuggerSnapshot *out_snapshot
);
UmiProtocolTransport *umi_studio_debugger_service_transport(
    UmiStudioDebuggerService *service
);
UmiDebugService *umi_studio_debugger_service_model(
    UmiStudioDebuggerService *service
);
UmiDebugController *umi_studio_debugger_service_controller(
    UmiStudioDebuggerService *service
);
UmiDebugWorkspace *umi_studio_debugger_service_workspace(
    UmiStudioDebuggerService *service
);

#ifdef __cplusplus
}
#endif

#endif

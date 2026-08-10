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
UmiStatus umi_studio_debugger_service_add_breakpoint(
    UmiStudioDebuggerService *service,
    const char *source_path,
    int line,
    int column
);
UmiStatus umi_studio_debugger_service_snapshot(
    const UmiStudioDebuggerService *service,
    UmiStudioDebuggerSnapshot *out_snapshot
);
UmiProtocolTransport *umi_studio_debugger_service_transport(
    UmiStudioDebuggerService *service
);

#ifdef __cplusplus
}
#endif

#endif

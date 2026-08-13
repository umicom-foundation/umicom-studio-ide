/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/language.h
 *
 * PURPOSE:
 *   Provide a toolkit-neutral Studio language service over JSON-RPC and Language Server Protocol contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_LANGUAGE_H
#define UMICOM_STUDIO_LANGUAGE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioLanguageService UmiStudioLanguageService;

typedef struct UmiStudioLanguageSnapshot {
    UmiProtocolClientState client_state;
    int initialized;
    size_t open_documents;
    size_t queued_messages;
    size_t sent_messages;
    size_t received_messages;
    size_t language_definitions;
    size_t diagnostics;
    size_t compilation_commands;
    size_t pending_requests;
    char session_state[64];
} UmiStudioLanguageSnapshot;

UmiStatus umi_studio_language_service_create(
    const char *root_uri,
    UmiStudioLanguageService **out_service
);
void umi_studio_language_service_destroy(UmiStudioLanguageService *service);
UmiStatus umi_studio_language_service_initialize(
    UmiStudioLanguageService *service,
    int64_t process_id,
    int64_t *out_request_id
);
UmiStatus umi_studio_language_service_open(
    UmiStudioLanguageService *service,
    const char *uri,
    const char *language_id,
    int version,
    const char *text
);
UmiStatus umi_studio_language_service_change(
    UmiStudioLanguageService *service,
    const char *uri,
    int version,
    const char *text
);
UmiStatus umi_studio_language_service_completion(
    UmiStudioLanguageService *service,
    const char *uri,
    UmiLspPosition position,
    int64_t *out_request_id
);
UmiStatus umi_studio_language_service_hover(
    UmiStudioLanguageService *service, const char *uri,
    UmiLspPosition position, int64_t *out_request_id
);
UmiStatus umi_studio_language_service_definition(
    UmiStudioLanguageService *service, const char *uri,
    UmiLspPosition position, int64_t *out_request_id
);
UmiStatus umi_studio_language_service_references(
    UmiStudioLanguageService *service, const char *uri,
    UmiLspPosition position, int include_declaration,
    int64_t *out_request_id
);
UmiStatus umi_studio_language_service_workspace_symbols(
    UmiStudioLanguageService *service, const char *query,
    int64_t *out_request_id
);
UmiStatus umi_studio_language_service_import_compilation_database(
    UmiStudioLanguageService *service, const char *json, size_t *out_imported
);
UmiStatus umi_studio_language_service_snapshot(
    const UmiStudioLanguageService *service,
    UmiStudioLanguageSnapshot *out_snapshot
);
UmiProtocolTransport *umi_studio_language_service_transport(
    UmiStudioLanguageService *service
);
UmiLanguageService *umi_studio_language_service_model(
    UmiStudioLanguageService *service
);

#ifdef __cplusplus
}
#endif

#endif

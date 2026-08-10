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
UmiStatus umi_studio_language_service_snapshot(
    const UmiStudioLanguageService *service,
    UmiStudioLanguageSnapshot *out_snapshot
);
UmiProtocolTransport *umi_studio_language_service_transport(
    UmiStudioLanguageService *service
);

#ifdef __cplusplus
}
#endif

#endif

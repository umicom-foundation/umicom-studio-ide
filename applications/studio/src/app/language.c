/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/language.c
 *
 * PURPOSE:
 *   Implement Studio language-server protocol state and document synchronisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/language.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioLanguageService {
    UmiProtocolTransport *transport;
    UmiProtocolClient *client;
    UmiLspClient lsp;
    UmiLspDocumentRegistry *documents;
};

UmiStatus umi_studio_language_service_create(
    const char *root_uri,
    UmiStudioLanguageService **out_service)
{
    UmiStudioLanguageService *service;
    UmiStatus status;
    if (root_uri == NULL || out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiStudioLanguageService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_protocol_transport_create_memory(256U, &service->transport);
    if (status == UMI_STATUS_OK) {
        status = umi_protocol_client_create(service->transport,
                                            &service->client);
    }
    if (status == UMI_STATUS_OK) status = umi_protocol_client_start(service->client);
    if (status == UMI_STATUS_OK) {
        status = umi_lsp_client_init(&service->lsp, service->client, root_uri);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_lsp_document_registry_create(&service->documents);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_language_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_language_service_destroy(UmiStudioLanguageService *service)
{
    if (service == NULL) return;
    umi_lsp_document_registry_destroy(service->documents);
    umi_protocol_client_destroy(service->client);
    umi_protocol_transport_destroy(service->transport);
    free(service);
}

UmiStatus umi_studio_language_service_initialize(
    UmiStudioLanguageService *service,
    int64_t process_id,
    int64_t *out_request_id)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_lsp_initialize(&service->lsp, process_id, out_request_id);
    if (status == UMI_STATUS_OK) status = umi_lsp_initialized(&service->lsp);
    return status;
}

UmiStatus umi_studio_language_service_open(
    UmiStudioLanguageService *service,
    const char *uri,
    const char *language_id,
    int version,
    const char *text)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_lsp_document_registry_open(service->documents,
                                            uri, language_id, version);
    if (status == UMI_STATUS_OK) {
        status = umi_lsp_did_open(&service->lsp,
                                  uri, language_id, version, text);
    }
    return status;
}

UmiStatus umi_studio_language_service_change(
    UmiStudioLanguageService *service,
    const char *uri,
    int version,
    const char *text)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_lsp_document_registry_change(service->documents, uri, version);
    if (status == UMI_STATUS_OK) {
        status = umi_lsp_did_change(&service->lsp, uri, version, text);
    }
    return status;
}

UmiStatus umi_studio_language_service_completion(
    UmiStudioLanguageService *service,
    const char *uri,
    UmiLspPosition position,
    int64_t *out_request_id)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_lsp_completion(&service->lsp, uri, position, out_request_id);
}

UmiStatus umi_studio_language_service_snapshot(
    const UmiStudioLanguageService *service,
    UmiStudioLanguageSnapshot *out_snapshot)
{
    UmiProtocolTransportStats stats;
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    stats = umi_protocol_transport_stats(service->transport);
    out_snapshot->client_state = umi_protocol_client_state(service->client);
    out_snapshot->initialized = service->lsp.initialized;
    out_snapshot->open_documents =
        umi_lsp_document_registry_count(service->documents);
    out_snapshot->queued_messages = stats.queued;
    out_snapshot->sent_messages = stats.sent;
    out_snapshot->received_messages = stats.received;
    return UMI_STATUS_OK;
}

UmiProtocolTransport *umi_studio_language_service_transport(
    UmiStudioLanguageService *service)
{
    return service != NULL ? service->transport : NULL;
}

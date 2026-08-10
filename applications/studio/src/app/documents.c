/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/documents.c
 *
 * PURPOSE:
 *   Implement Studio document creation, opening, atomic saving, session
 *   tracking and crash-recovery checkpoints through Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/documents.h"

#include <string.h>

#include "umicom/studio/session.h"

static const char *display_name_from_path(const char *path)
{
    const char *name = strrchr(path, '/');
#ifdef _WIN32
    const char *backslash = strrchr(path, '\\');
    if (backslash != NULL && (name == NULL || backslash > name)) {
        name = backslash;
    }
#endif
    return name != NULL ? name + 1 : path;
}

UmiStatus umi_studio_documents_new(UmiStudioServices *services,
                                   const char *display_name,
                                   UmiDocumentId *out_document_id)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiStatus status;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_new(documents,
                                    display_name,
                                    out_document_id);
    if (status == UMI_STATUS_OK) {
        (void)umi_studio_session_set_active_document(services,
                                                     *out_document_id);
    }
    return status;
}

UmiStatus umi_studio_documents_open(UmiStudioServices *services,
                                    const char *path,
                                    UmiDocumentId *out_document_id)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiStatus status;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_open(documents, path, out_document_id);
    if (status == UMI_STATUS_OK) {
        (void)umi_studio_session_remember_document(services,
                                                   *out_document_id,
                                                   path);
        (void)umi_studio_session_set_active_document(services,
                                                     *out_document_id);
    }
    return status;
}

UmiStatus umi_studio_documents_replace(UmiStudioServices *services,
                                       UmiDocumentId document_id,
                                       const char *text,
                                       size_t length)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_document_store_replace_text(documents,
                                           document_id,
                                           text,
                                           length);
}

static UmiStatus remove_recovery(UmiStudioServices *services,
                                 UmiDocumentId document_id)
{
    UmiRecoveryManager *recovery = umi_studio_services_recovery(services);
    if (recovery == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_recovery_manager_remove(recovery, document_id);
}

UmiStatus umi_studio_documents_save(UmiStudioServices *services,
                                    UmiDocumentId document_id)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiStatus status;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_save(documents, document_id);
    if (status == UMI_STATUS_OK) {
        (void)remove_recovery(services, document_id);
    }
    return status;
}

UmiStatus umi_studio_documents_save_as(UmiStudioServices *services,
                                       UmiDocumentId document_id,
                                       const char *path)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiStatus status;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_save_as(documents, document_id, path);
    if (status == UMI_STATUS_OK) {
        (void)umi_studio_session_remember_document(services,
                                                   document_id,
                                                   path);
        (void)remove_recovery(services, document_id);
    }
    return status;
}

UmiStatus umi_studio_documents_close(UmiStudioServices *services,
                                     UmiDocumentId document_id,
                                     int force)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiStatus status;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_close(documents, document_id, force);
    if (status == UMI_STATUS_OK) {
        (void)umi_studio_session_forget_document(services, document_id);
        (void)remove_recovery(services, document_id);
    }
    return status;
}

UmiStatus umi_studio_documents_checkpoint(UmiStudioServices *services,
                                          UmiDocumentId document_id)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiRecoveryManager *recovery = umi_studio_services_recovery(services);
    UmiDocumentSnapshot snapshot;
    char *text = NULL;
    size_t length = 0U;
    UmiStatus status;

    if (documents == NULL || recovery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_store_snapshot(documents,
                                         document_id,
                                         &snapshot);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (!snapshot.dirty) {
        return remove_recovery(services, document_id);
    }

    status = umi_document_store_copy_text(documents,
                                          document_id,
                                          &text,
                                          &length);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_recovery_manager_save(recovery,
                                       document_id,
                                       snapshot.path,
                                       snapshot.revision,
                                       text,
                                       length);
    umi_document_store_free_text(text);
    return status;
}

UmiStatus umi_studio_documents_restore_recovery(
    UmiStudioServices *services,
    UmiDocumentId recovery_document_id,
    UmiDocumentId *out_document_id)
{
    UmiRecoveryManager *recovery = umi_studio_services_recovery(services);
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    UmiRecoveryRecord record;
    UmiStatus status;
    const char *display_name;

    if (recovery == NULL || documents == NULL || out_document_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_document_id = 0U;

    status = umi_recovery_manager_load(recovery,
                                       recovery_document_id,
                                       &record);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    display_name = record.source_path[0] != '\0'
        ? display_name_from_path(record.source_path)
        : "Recovered document";
    status = umi_document_store_new(documents,
                                    display_name,
                                    out_document_id);
    if (status == UMI_STATUS_OK) {
        status = umi_document_store_replace_text(documents,
                                                 *out_document_id,
                                                 record.text,
                                                 record.length);
    }
    if (status == UMI_STATUS_OK && record.source_path[0] != '\0') {
        (void)umi_studio_session_remember_document(services,
                                                   *out_document_id,
                                                   record.source_path);
    }
    umi_recovery_record_dispose(&record);
    return status;
}

UmiStatus umi_studio_documents_save_all(UmiStudioServices *services,
                                        size_t *out_saved,
                                        size_t *out_failed)
{
    UmiDocumentStore *documents = umi_studio_services_documents(services);
    size_t saved = 0U;
    size_t failed = 0U;
    size_t index;
    size_t count;

    if (documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_document_store_count(documents);
    for (index = 0U; index < count; ++index) {
        UmiDocumentSnapshot snapshot;
        UmiStatus status = umi_document_store_at(documents,
                                                 index,
                                                 &snapshot);
        if (status != UMI_STATUS_OK) {
            failed += 1U;
            continue;
        }
        if (!snapshot.dirty) {
            continue;
        }
        if (!snapshot.has_path) {
            failed += 1U;
            continue;
        }
        status = umi_studio_documents_save(services,
                                           snapshot.document_id);
        if (status == UMI_STATUS_OK) {
            saved += 1U;
        } else {
            failed += 1U;
        }
    }

    if (out_saved != NULL) {
        *out_saved = saved;
    }
    if (out_failed != NULL) {
        *out_failed = failed;
    }
    return failed == 0U ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

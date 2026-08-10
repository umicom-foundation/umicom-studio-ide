/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/documents.h
 *
 * PURPOSE:
 *   Define Studio document use cases over the Framework document and recovery
 *   services so console, GTK4 and future frontends share identical behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DOCUMENTS_H
#define UMICOM_STUDIO_DOCUMENTS_H

#include <stddef.h>

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_documents_new(UmiStudioServices *services,
                                   const char *display_name,
                                   UmiDocumentId *out_document_id);
UmiStatus umi_studio_documents_open(UmiStudioServices *services,
                                    const char *path,
                                    UmiDocumentId *out_document_id);
UmiStatus umi_studio_documents_replace(UmiStudioServices *services,
                                       UmiDocumentId document_id,
                                       const char *text,
                                       size_t length);
UmiStatus umi_studio_documents_save(UmiStudioServices *services,
                                    UmiDocumentId document_id);
UmiStatus umi_studio_documents_save_as(UmiStudioServices *services,
                                       UmiDocumentId document_id,
                                       const char *path);
UmiStatus umi_studio_documents_close(UmiStudioServices *services,
                                     UmiDocumentId document_id,
                                     int force);
UmiStatus umi_studio_documents_checkpoint(UmiStudioServices *services,
                                          UmiDocumentId document_id);
UmiStatus umi_studio_documents_restore_recovery(
    UmiStudioServices *services,
    UmiDocumentId recovery_document_id,
    UmiDocumentId *out_document_id
);
UmiStatus umi_studio_documents_save_all(UmiStudioServices *services,
                                        size_t *out_saved,
                                        size_t *out_failed);

#ifdef __cplusplus
}
#endif

#endif

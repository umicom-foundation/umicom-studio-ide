/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/document_set.h
 *
 * PURPOSE:
 *   Manage bounded open-editor documents and active-tab state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#ifndef UMICOM_STUDIO_DOCUMENT_SET_H
#define UMICOM_STUDIO_DOCUMENT_SET_H

#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDocumentSet {
    UmiStudioWorkspaceDocument documents[UMI_STUDIO_WORKSPACE_MAX_DOCUMENTS];
    size_t count;
    uint64_t usage_tick;
} UmiStudioDocumentSet;

void umi_studio_document_set_init(UmiStudioDocumentSet *set);

UmiStatus umi_studio_document_set_open(
    UmiStudioDocumentSet *set,
    const char *path,
    const char *language,
    bool preview
);

UmiStudioWorkspaceDocument *umi_studio_document_set_find(
    UmiStudioDocumentSet *set,
    const char *path
);

const UmiStudioWorkspaceDocument *umi_studio_document_set_find_const(
    const UmiStudioDocumentSet *set,
    const char *path
);

UmiStatus umi_studio_document_set_activate(
    UmiStudioDocumentSet *set,
    const char *path
);

UmiStudioWorkspaceDocument *umi_studio_document_set_active(
    UmiStudioDocumentSet *set
);

UmiStatus umi_studio_document_set_mark_dirty(
    UmiStudioDocumentSet *set,
    const char *path,
    bool dirty
);

UmiStatus umi_studio_document_set_pin(
    UmiStudioDocumentSet *set,
    const char *path,
    bool pinned
);

UmiStatus umi_studio_document_set_close(
    UmiStudioDocumentSet *set,
    const char *path,
    bool force
);

size_t umi_studio_document_set_dirty_count(const UmiStudioDocumentSet *set);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/document_set.c
 *
 * PURPOSE:
 *   Implement open-editor lifecycle, preview replacement, activation, pinning and dirty-close protection.
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

#include "umicom/studio/document_set.h"

#include <string.h>

void umi_studio_document_set_init(UmiStudioDocumentSet *set)
{
    if (set != NULL) {
        (void)memset(set, 0, sizeof(*set));
    }
}

UmiStudioWorkspaceDocument *umi_studio_document_set_find(
    UmiStudioDocumentSet *set,
    const char *path)
{
    size_t index;

    if (set == NULL || path == NULL) {
        return NULL;
    }

    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->documents[index].path, path) == 0) {
            return &set->documents[index];
        }
    }

    return NULL;
}

const UmiStudioWorkspaceDocument *umi_studio_document_set_find_const(
    const UmiStudioDocumentSet *set,
    const char *path)
{
    size_t index;

    if (set == NULL || path == NULL) {
        return NULL;
    }

    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->documents[index].path, path) == 0) {
            return &set->documents[index];
        }
    }

    return NULL;
}

static void activate_document(UmiStudioDocumentSet *set,
                              UmiStudioWorkspaceDocument *document)
{
    size_t index;

    for (index = 0U; index < set->count; ++index) {
        set->documents[index].active = false;
    }

    document->active = true;
    document->last_used_tick = ++set->usage_tick;
}

static UmiStudioWorkspaceDocument *replaceable_preview(UmiStudioDocumentSet *set)
{
    size_t index;

    for (index = 0U; index < set->count; ++index) {
        UmiStudioWorkspaceDocument *document = &set->documents[index];
        if (document->preview && !document->pinned && !document->dirty) {
            return document;
        }
    }

    return NULL;
}

UmiStatus umi_studio_document_set_open(
    UmiStudioDocumentSet *set,
    const char *path,
    const char *language,
    bool preview)
{
    UmiStudioWorkspaceDocument *document;
    UmiStatus status;

    if (set == NULL || path == NULL || language == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    document = umi_studio_document_set_find(set, path);
    if (document != NULL) {
        activate_document(set, document);
        return UMI_STATUS_OK;
    }

    if (preview) {
        document = replaceable_preview(set);
        if (document != NULL) {
            umi_studio_workspace_document_init(document);
        }
    } else {
        document = NULL;
    }

    if (document == NULL) {
        if (set->count >= UMI_STUDIO_WORKSPACE_MAX_DOCUMENTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        document = &set->documents[set->count++];
        umi_studio_workspace_document_init(document);
    }

    status = umi_studio_workspace_document_set_path(document, path);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_studio_workspace_document_set_language(document, language);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    document->preview = preview;
    activate_document(set, document);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_document_set_activate(
    UmiStudioDocumentSet *set,
    const char *path)
{
    UmiStudioWorkspaceDocument *document;

    if (set == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    document = umi_studio_document_set_find(set, path);
    if (document == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    activate_document(set, document);
    return UMI_STATUS_OK;
}

UmiStudioWorkspaceDocument *umi_studio_document_set_active(
    UmiStudioDocumentSet *set)
{
    size_t index;

    if (set == NULL) {
        return NULL;
    }

    for (index = 0U; index < set->count; ++index) {
        if (set->documents[index].active) {
            return &set->documents[index];
        }
    }

    return NULL;
}

UmiStatus umi_studio_document_set_mark_dirty(
    UmiStudioDocumentSet *set,
    const char *path,
    bool dirty)
{
    UmiStudioWorkspaceDocument *document = umi_studio_document_set_find(set, path);

    if (document == NULL) {
        return set == NULL || path == NULL
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    document->dirty = dirty;
    if (dirty) {
        document->preview = false;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_document_set_pin(
    UmiStudioDocumentSet *set,
    const char *path,
    bool pinned)
{
    UmiStudioWorkspaceDocument *document = umi_studio_document_set_find(set, path);

    if (document == NULL) {
        return set == NULL || path == NULL
            ? UMI_STATUS_INVALID_ARGUMENT
            : UMI_STATUS_NOT_FOUND;
    }

    document->pinned = pinned;
    if (pinned) {
        document->preview = false;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_document_set_close(
    UmiStudioDocumentSet *set,
    const char *path,
    bool force)
{
    size_t index;

    if (set == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->documents[index].path, path) == 0) {
            bool was_active = set->documents[index].active;

            if (set->documents[index].dirty && !force) {
                return UMI_STATUS_BUSY;
            }

            if (index + 1U < set->count) {
                (void)memmove(&set->documents[index],
                              &set->documents[index + 1U],
                              (set->count - index - 1U) *
                                  sizeof(set->documents[0]));
            }
            --set->count;

            if (was_active && set->count > 0U) {
                set->documents[set->count - 1U].active = true;
                set->documents[set->count - 1U].last_used_tick =
                    ++set->usage_tick;
            }

            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

size_t umi_studio_document_set_dirty_count(const UmiStudioDocumentSet *set)
{
    size_t count = 0U;
    size_t index;

    if (set == NULL) {
        return 0U;
    }

    for (index = 0U; index < set->count; ++index) {
        if (set->documents[index].dirty) {
            ++count;
        }
    }

    return count;
}

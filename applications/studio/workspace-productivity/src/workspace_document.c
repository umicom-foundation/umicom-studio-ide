/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/workspace_document.c
 *
 * PURPOSE:
 *   Implement path/name/language/cursor state for one open editor document.
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

#include "umicom/studio/workspace_document.h"

#include <stdio.h>
#include <string.h>

static const char *basename_from_path(const char *path)
{
    const char *slash;
    const char *backslash;
    const char *last;

    if (path == NULL) {
        return "";
    }

    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    last = slash;

    if (backslash != NULL && (last == NULL || backslash > last)) {
        last = backslash;
    }

    return last != NULL ? last + 1 : path;
}

void umi_studio_workspace_document_init(UmiStudioWorkspaceDocument *document)
{
    if (document != NULL) {
        (void)memset(document, 0, sizeof(*document));
        document->line = 1U;
        document->column = 1U;
    }
}

UmiStatus umi_studio_workspace_document_set_path(
    UmiStudioWorkspaceDocument *document,
    const char *path)
{
    int written;

    if (document == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(document->path, sizeof(document->path), "%s", path);
    if (written < 0 || (size_t)written >= sizeof(document->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    written = snprintf(document->display_name,
                       sizeof(document->display_name),
                       "%s",
                       basename_from_path(path));
    if (written < 0 || (size_t)written >= sizeof(document->display_name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_workspace_document_set_language(
    UmiStudioWorkspaceDocument *document,
    const char *language)
{
    int written;

    if (document == NULL || language == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(document->language,
                       sizeof(document->language),
                       "%s",
                       language);
    return written >= 0 && (size_t)written < sizeof(document->language)
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

void umi_studio_workspace_document_set_cursor(
    UmiStudioWorkspaceDocument *document,
    size_t line,
    size_t column)
{
    if (document == NULL) {
        return;
    }

    document->line = line == 0U ? 1U : line;
    document->column = column == 0U ? 1U : column;
}

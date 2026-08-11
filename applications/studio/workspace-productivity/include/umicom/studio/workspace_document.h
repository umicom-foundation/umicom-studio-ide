/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/workspace_document.h
 *
 * PURPOSE:
 *   Define one open editor document with cursor, dirty, pin and preview state.
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

#ifndef UMICOM_STUDIO_WORKSPACE_DOCUMENT_H
#define UMICOM_STUDIO_WORKSPACE_DOCUMENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_WORKSPACE_PATH_CAPACITY 1024U
#define UMI_STUDIO_WORKSPACE_NAME_CAPACITY 256U
#define UMI_STUDIO_WORKSPACE_LANGUAGE_CAPACITY 64U
#define UMI_STUDIO_WORKSPACE_MAX_DOCUMENTS 96U

typedef struct UmiStudioWorkspaceDocument {
    char path[UMI_STUDIO_WORKSPACE_PATH_CAPACITY];
    char display_name[UMI_STUDIO_WORKSPACE_NAME_CAPACITY];
    char language[UMI_STUDIO_WORKSPACE_LANGUAGE_CAPACITY];
    size_t line;
    size_t column;
    uint64_t last_used_tick;
    bool dirty;
    bool pinned;
    bool preview;
    bool active;
} UmiStudioWorkspaceDocument;

void umi_studio_workspace_document_init(UmiStudioWorkspaceDocument *document);

UmiStatus umi_studio_workspace_document_set_path(
    UmiStudioWorkspaceDocument *document,
    const char *path
);

UmiStatus umi_studio_workspace_document_set_language(
    UmiStudioWorkspaceDocument *document,
    const char *language
);

void umi_studio_workspace_document_set_cursor(
    UmiStudioWorkspaceDocument *document,
    size_t line,
    size_t column
);

#ifdef __cplusplus
}
#endif

#endif

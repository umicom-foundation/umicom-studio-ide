/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/workspace_summary.h
 *
 * PURPOSE:
 *   Summarise open, dirty, pinned, preview and recent-workspace counts.
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

#ifndef UMICOM_STUDIO_WORKSPACE_SUMMARY_H
#define UMICOM_STUDIO_WORKSPACE_SUMMARY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/document_set.h"
#include "umicom/studio/recent_workspaces.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioWorkspaceSummary {
    size_t open_documents;
    size_t dirty_documents;
    size_t pinned_documents;
    size_t preview_documents;
    size_t recent_workspaces;
} UmiStudioWorkspaceSummary;

UmiStatus umi_studio_workspace_summary_build(
    const UmiStudioDocumentSet *documents,
    const UmiStudioRecentWorkspaceList *recent,
    UmiStudioWorkspaceSummary *summary
);

#ifdef __cplusplus
}
#endif

#endif

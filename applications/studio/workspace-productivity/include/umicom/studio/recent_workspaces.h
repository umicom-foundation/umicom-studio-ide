/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/recent_workspaces.h
 *
 * PURPOSE:
 *   Maintain a bounded recent-workspace MRU list.
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

#ifndef UMICOM_STUDIO_RECENT_WORKSPACES_H
#define UMICOM_STUDIO_RECENT_WORKSPACES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_RECENT_WORKSPACE_MAX 32U

typedef struct UmiStudioRecentWorkspace {
    char path[UMI_STUDIO_WORKSPACE_PATH_CAPACITY];
    char name[UMI_STUDIO_WORKSPACE_NAME_CAPACITY];
    uint64_t last_opened_tick;
    bool pinned;
} UmiStudioRecentWorkspace;

typedef struct UmiStudioRecentWorkspaceList {
    UmiStudioRecentWorkspace items[UMI_STUDIO_RECENT_WORKSPACE_MAX];
    size_t count;
    uint64_t tick;
} UmiStudioRecentWorkspaceList;

void umi_studio_recent_workspaces_init(UmiStudioRecentWorkspaceList *list);

UmiStatus umi_studio_recent_workspaces_touch(
    UmiStudioRecentWorkspaceList *list,
    const char *path,
    const char *name
);

UmiStatus umi_studio_recent_workspaces_pin(
    UmiStudioRecentWorkspaceList *list,
    const char *path,
    bool pinned
);

const UmiStudioRecentWorkspace *umi_studio_recent_workspaces_at(
    const UmiStudioRecentWorkspaceList *list,
    size_t index
);

#ifdef __cplusplus
}
#endif

#endif

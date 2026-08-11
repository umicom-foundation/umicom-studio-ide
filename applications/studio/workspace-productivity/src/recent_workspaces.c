/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/recent_workspaces.c
 *
 * PURPOSE:
 *   Implement recent-workspace MRU ordering and pinning.
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

#include "umicom/studio/recent_workspaces.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

void umi_studio_recent_workspaces_init(UmiStudioRecentWorkspaceList *list)
{
    if (list != NULL) {
        (void)memset(list, 0, sizeof(*list));
    }
}

static size_t find_index(const UmiStudioRecentWorkspaceList *list,
                         const char *path)
{
    size_t index;

    for (index = 0U; index < list->count; ++index) {
        if (strcmp(list->items[index].path, path) == 0) {
            return index;
        }
    }

    return list->count;
}

static void move_to_front(UmiStudioRecentWorkspaceList *list, size_t index)
{
    UmiStudioRecentWorkspace item;

    if (index == 0U || index >= list->count) {
        return;
    }

    item = list->items[index];
    (void)memmove(&list->items[1],
                  &list->items[0],
                  index * sizeof(list->items[0]));
    list->items[0] = item;
}

UmiStatus umi_studio_recent_workspaces_touch(
    UmiStudioRecentWorkspaceList *list,
    const char *path,
    const char *name)
{
    size_t index;
    UmiStatus status;

    if (list == NULL || path == NULL || name == NULL ||
        path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(list, path);

    if (index == list->count) {
        if (list->count < UMI_STUDIO_RECENT_WORKSPACE_MAX) {
            ++list->count;
        } else {
            index = list->count - 1U;
        }

        if (index >= list->count) {
            index = list->count - 1U;
        }

        (void)memset(&list->items[index], 0, sizeof(list->items[index]));
    }

    status = copy_text(list->items[index].path,
                       sizeof(list->items[index].path),
                       path);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = copy_text(list->items[index].name,
                       sizeof(list->items[index].name),
                       name);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    list->items[index].last_opened_tick = ++list->tick;
    move_to_front(list, index);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_recent_workspaces_pin(
    UmiStudioRecentWorkspaceList *list,
    const char *path,
    bool pinned)
{
    size_t index;

    if (list == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(list, path);
    if (index >= list->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    list->items[index].pinned = pinned;
    return UMI_STATUS_OK;
}

const UmiStudioRecentWorkspace *umi_studio_recent_workspaces_at(
    const UmiStudioRecentWorkspaceList *list,
    size_t index)
{
    return list != NULL && index < list->count
        ? &list->items[index]
        : NULL;
}

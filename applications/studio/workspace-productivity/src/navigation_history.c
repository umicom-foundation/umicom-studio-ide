/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/navigation_history.c
 *
 * PURPOSE:
 *   Implement editor navigation history with standard back/forward truncation semantics.
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

#include "umicom/studio/navigation_history.h"

#include <stdio.h>
#include <string.h>

void umi_studio_navigation_history_init(UmiStudioNavigationHistory *history)
{
    if (history != NULL) {
        (void)memset(history, 0, sizeof(*history));
    }
}

UmiStatus umi_studio_navigation_history_push(
    UmiStudioNavigationHistory *history,
    const char *path,
    size_t line,
    size_t column)
{
    UmiStudioNavigationLocation *location;
    int written;

    if (history == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (history->count > 0U && history->cursor + 1U < history->count) {
        history->count = history->cursor + 1U;
    }

    if (history->count == UMI_STUDIO_NAVIGATION_MAX) {
        (void)memmove(&history->items[0],
                      &history->items[1],
                      (history->count - 1U) * sizeof(history->items[0]));
        --history->count;
    }

    location = &history->items[history->count];
    (void)memset(location, 0, sizeof(*location));
    written = snprintf(location->path, sizeof(location->path), "%s", path);
    if (written < 0 || (size_t)written >= sizeof(location->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    location->line = line == 0U ? 1U : line;
    location->column = column == 0U ? 1U : column;

    history->cursor = history->count;
    ++history->count;
    return UMI_STATUS_OK;
}

const UmiStudioNavigationLocation *umi_studio_navigation_history_current(
    const UmiStudioNavigationHistory *history)
{
    return history != NULL && history->count > 0U &&
           history->cursor < history->count
        ? &history->items[history->cursor]
        : NULL;
}

const UmiStudioNavigationLocation *umi_studio_navigation_history_back(
    UmiStudioNavigationHistory *history)
{
    if (history == NULL || history->count == 0U || history->cursor == 0U) {
        return NULL;
    }

    --history->cursor;
    return &history->items[history->cursor];
}

const UmiStudioNavigationLocation *umi_studio_navigation_history_forward(
    UmiStudioNavigationHistory *history)
{
    if (history == NULL || history->count == 0U ||
        history->cursor + 1U >= history->count) {
        return NULL;
    }

    ++history->cursor;
    return &history->items[history->cursor];
}

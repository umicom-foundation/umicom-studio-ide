/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/navigation_history.h
 *
 * PURPOSE:
 *   Provide editor back/forward location history.
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

#ifndef UMICOM_STUDIO_NAVIGATION_HISTORY_H
#define UMICOM_STUDIO_NAVIGATION_HISTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_NAVIGATION_MAX 128U

typedef struct UmiStudioNavigationLocation {
    char path[UMI_STUDIO_WORKSPACE_PATH_CAPACITY];
    size_t line;
    size_t column;
} UmiStudioNavigationLocation;

typedef struct UmiStudioNavigationHistory {
    UmiStudioNavigationLocation items[UMI_STUDIO_NAVIGATION_MAX];
    size_t count;
    size_t cursor;
} UmiStudioNavigationHistory;

void umi_studio_navigation_history_init(UmiStudioNavigationHistory *history);

UmiStatus umi_studio_navigation_history_push(
    UmiStudioNavigationHistory *history,
    const char *path,
    size_t line,
    size_t column
);

const UmiStudioNavigationLocation *umi_studio_navigation_history_current(
    const UmiStudioNavigationHistory *history
);

const UmiStudioNavigationLocation *umi_studio_navigation_history_back(
    UmiStudioNavigationHistory *history
);

const UmiStudioNavigationLocation *umi_studio_navigation_history_forward(
    UmiStudioNavigationHistory *history
);

#ifdef __cplusplus
}
#endif

#endif

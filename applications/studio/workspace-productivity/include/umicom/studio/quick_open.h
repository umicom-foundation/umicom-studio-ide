/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/quick_open.h
 *
 * PURPOSE:
 *   Provide fuzzy quick-open scoring and ranked results.
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

#ifndef UMICOM_STUDIO_QUICK_OPEN_H
#define UMICOM_STUDIO_QUICK_OPEN_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_QUICK_OPEN_MAX_CANDIDATES 256U
#define UMI_STUDIO_QUICK_OPEN_MAX_RESULTS 32U

typedef enum UmiStudioQuickOpenKind {
    UMI_STUDIO_QUICK_OPEN_DOCUMENT = 0,
    UMI_STUDIO_QUICK_OPEN_FILE,
    UMI_STUDIO_QUICK_OPEN_WORKSPACE,
    UMI_STUDIO_QUICK_OPEN_SYMBOL
} UmiStudioQuickOpenKind;

typedef struct UmiStudioQuickOpenCandidate {
    char label[UMI_STUDIO_WORKSPACE_NAME_CAPACITY];
    char detail[UMI_STUDIO_WORKSPACE_PATH_CAPACITY];
    UmiStudioQuickOpenKind kind;
    int score;
} UmiStudioQuickOpenCandidate;

typedef struct UmiStudioQuickOpenResults {
    UmiStudioQuickOpenCandidate items[UMI_STUDIO_QUICK_OPEN_MAX_RESULTS];
    size_t count;
} UmiStudioQuickOpenResults;

int umi_studio_quick_open_score(const char *query, const char *candidate);

UmiStatus umi_studio_quick_open_search(
    const char *query,
    const UmiStudioQuickOpenCandidate *candidates,
    size_t candidate_count,
    UmiStudioQuickOpenResults *results
);

#ifdef __cplusplus
}
#endif

#endif

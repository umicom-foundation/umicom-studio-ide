/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/tab_policy.h
 *
 * PURPOSE:
 *   Centralise tab labels and dirty-close decisions.
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

#ifndef UMICOM_STUDIO_TAB_POLICY_H
#define UMICOM_STUDIO_TAB_POLICY_H

#include <stdbool.h>
#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/workspace_document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiStudioTabCloseDecision {
    UMI_STUDIO_TAB_CLOSE_ALLOWED = 0,
    UMI_STUDIO_TAB_CLOSE_CONFIRM_DIRTY,
    UMI_STUDIO_TAB_CLOSE_NOT_FOUND
} UmiStudioTabCloseDecision;

UmiStudioTabCloseDecision umi_studio_tab_close_decision(
    const UmiStudioWorkspaceDocument *document
);

UmiStatus umi_studio_tab_label(
    const UmiStudioWorkspaceDocument *document,
    char *output,
    size_t output_capacity
);

#ifdef __cplusplus
}
#endif

#endif

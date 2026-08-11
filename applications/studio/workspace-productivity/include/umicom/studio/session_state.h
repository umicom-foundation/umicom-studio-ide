/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/include/umicom/studio/session_state.h
 *
 * PURPOSE:
 *   Serialize and restore workspace/document session state.
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

#ifndef UMICOM_STUDIO_SESSION_STATE_H
#define UMICOM_STUDIO_SESSION_STATE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/studio/document_set.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_SESSION_TEXT_CAPACITY 32768U

typedef struct UmiStudioSessionState {
    char workspace_path[UMI_STUDIO_WORKSPACE_PATH_CAPACITY];
    UmiStudioDocumentSet documents;
} UmiStudioSessionState;

void umi_studio_session_state_init(UmiStudioSessionState *state);

UmiStatus umi_studio_session_state_set_workspace(
    UmiStudioSessionState *state,
    const char *workspace_path
);

UmiStatus umi_studio_session_state_encode(
    const UmiStudioSessionState *state,
    char *output,
    size_t output_capacity
);

UmiStatus umi_studio_session_state_decode(
    const char *text,
    UmiStudioSessionState *state
);

#ifdef __cplusplus
}
#endif

#endif

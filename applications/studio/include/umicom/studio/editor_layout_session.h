/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/editor_layout_session.h
 *
 * PURPOSE:
 *   Persist Framework editor-group ordering through Studio's existing
 *   workspace-local Session Store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_EDITOR_LAYOUT_SESSION_H
#define UMICOM_STUDIO_EDITOR_LAYOUT_SESSION_H

#include "umicom/platform/session_store.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_editor_layout_session_restore(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session,
    UmiUiDocumentLayoutApplyResult *out_result);
UmiStatus umi_studio_editor_layout_session_save(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session);

#ifdef __cplusplus
}
#endif

#endif

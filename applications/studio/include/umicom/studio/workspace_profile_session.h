/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workspace_profile_session.h
 *
 * PURPOSE:
 *   Persist user-created Framework workspace profiles through Studio's
 *   existing Session Store without introducing another settings backend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_WORKSPACE_PROFILE_SESSION_H
#define UMICOM_STUDIO_WORKSPACE_PROFILE_SESSION_H

#include "umicom/platform/session_store.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_workspace_profile_session_restore(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session);
UmiStatus umi_studio_workspace_profile_session_save(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session);

#ifdef __cplusplus
}
#endif

#endif

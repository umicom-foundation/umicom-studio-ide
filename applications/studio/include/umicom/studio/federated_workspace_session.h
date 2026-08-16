/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/federated_workspace_session.h
 *
 * PURPOSE:
 *   Declare Studio's session identity at the Framework composition boundary.
 *   Framework owns recovery journals, layout history and monitor affinity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FEDERATED_WORKSPACE_SESSION_H
#define UMICOM_STUDIO_FEDERATED_WORKSPACE_SESSION_H

#include "umicom/desktop/monitor_interaction.h"
#include "umicom/desktop/session_recovery.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_federated_workspace_session_begin(
    UmiDesktopSessionRecovery *recovery,
    UmiDesktopMonitorInteraction *monitor_interaction);
UmiStatus umi_studio_federated_workspace_session_end(
    UmiDesktopSessionRecovery *recovery);

#ifdef __cplusplus
}
#endif

#endif

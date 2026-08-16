/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/federated_workspace_session.c
 *
 * PURPOSE:
 *   Supply Studio's declarative session identity to Framework services. All
 *   checkpoint, transaction and monitor-affinity logic remains in Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/federated_workspace_session.h"

UmiStatus umi_studio_federated_workspace_session_begin(
    UmiDesktopSessionRecovery *recovery,
    UmiDesktopMonitorInteraction *monitor_interaction)
{
    UmiStatus status;
    if (recovery == NULL || monitor_interaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_desktop_monitor_interaction_capture_affinities(
        monitor_interaction);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_desktop_session_recovery_begin(
        recovery, "studio-desktop", "org.umicom.studio");
}

UmiStatus umi_studio_federated_workspace_session_end(
    UmiDesktopSessionRecovery *recovery)
{
    if (recovery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_desktop_session_recovery_mark_clean(recovery);
}

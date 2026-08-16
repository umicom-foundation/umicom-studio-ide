/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_federated_workspace_session.c
 *
 * PURPOSE:
 *   Verify Studio composes Framework-owned history, recovery and monitor
 *   services without implementing those behaviours in the application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/bootstrap.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDesktopRuntime *runtime;
    UmiDesktopLayoutHistory *history;
    UmiDesktopSessionRecovery *recovery;
    UmiDesktopMonitorInteraction *monitor_interaction;
    UmiDesktopLayoutHistorySnapshot history_snapshot;
    UmiDesktopSessionRecoverySnapshot recovery_snapshot;
    const UmiDesktopWindow *window;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    runtime = umi_studio_bootstrap_desktop_runtime(bootstrap);
    history = umi_studio_bootstrap_layout_history(bootstrap);
    recovery = umi_studio_bootstrap_session_recovery(bootstrap);
    monitor_interaction = umi_studio_bootstrap_monitor_interaction(bootstrap);
    assert(runtime != NULL && history != NULL && recovery != NULL);
    assert(monitor_interaction != NULL);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_snapshot(
        recovery, &recovery_snapshot) == UMI_STATUS_OK);
    assert(recovery_snapshot.state == UMI_DESKTOP_SESSION_RECOVERY_RUNNING);
    assert(recovery_snapshot.checkpoint_count == 1U);

    assert(umi_desktop_runtime_clone_layout(
        runtime, "develop", "studio-session", "Studio Session", true) ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_history_begin(
        history, "studio-layout-1", "umicom.desktop.window.move",
        "org.umicom.studio", "Move project explorer") == UMI_STATUS_OK);
    assert(umi_desktop_runtime_place_window(
        runtime, "project-explorer", "primary",
        (UmiDesktopRect){240, 120, 460, 720},
        UMI_DESKTOP_DOCK_FLOATING) == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_commit(history, "studio-layout-1") ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_history_undo(history) == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_redo(history) == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_snapshot(
        history, &history_snapshot) == UMI_STATUS_OK);
    assert(history_snapshot.entry_count == 1U);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && window->bounds.x == 240);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_snapshot(
        recovery, &recovery_snapshot) == UMI_STATUS_OK);
    assert(recovery_snapshot.state == UMI_DESKTOP_SESSION_RECOVERY_CLEAN);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

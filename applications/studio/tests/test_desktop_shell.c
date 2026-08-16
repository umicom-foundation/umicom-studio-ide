/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_desktop_shell.c
 *
 * PURPOSE:
 *   Verify Studio remains a thin consumer of the Master Controller-owned
 *   federated desktop shell and Framework visual layout designer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDesktopShellModel *shell;
    UmiDesktopShellSnapshot snapshot;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    shell = umi_studio_bootstrap_desktop_shell(bootstrap);
    assert(shell != NULL);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.tab_count == 16U);
    assert(strcmp(snapshot.active_layout_id, "develop") == 0);
    assert(umi_desktop_shell_model_activate_layout(shell, "trading") ==
           UMI_STATUS_OK);
    assert(umi_desktop_shell_model_begin_design(
        shell, "studio-trading-b79", "Studio Trading") == UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_select(
        umi_desktop_shell_model_designer(shell), "trading-chart") ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_dock_selected(
        umi_desktop_shell_model_designer(shell), UMI_DESKTOP_DOCK_RIGHT) ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_view_create(
        "studio.desktop-designer", shell, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "designer-active", &value) == UMI_STATUS_OK);
    assert(value.boolean_value);
    umi_ui_view_model_destroy(view);
    assert(umi_desktop_shell_model_end_design(shell, false) == UMI_STATUS_OK);
    assert(umi_desktop_shell_model_snapshot(shell, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.active_layout_id, "trading") == 0);
    assert(!snapshot.designer_active);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio terminal workbench composition tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"

static void assert_view_kind(UmiUiWorkbench *workbench,
                             const char *view_type,
                             const char *pane_id,
                             const char *expected)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_ui_view_factory_create_view(
        umi_ui_workbench_view_factories(workbench), view_type, pane_id, &view) ==
        UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "umicom.view-kind", &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiTerminalController *controller;
    UmiTerminalControllerSnapshot snapshot;
    UmiUiWorkbench *workbench;
    UmiUiPaneSnapshot pane;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    controller = umi_studio_services_terminal_controller(services);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(controller != NULL && workbench != NULL);
    assert(umi_terminal_controller_snapshot(controller, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.sessions == 1U && snapshot.tabs == 1U);

    assert_view_kind(workbench, "studio.terminal",
                     UMI_STUDIO_PANE_TERMINAL, "terminal");
    assert_view_kind(workbench, "studio.processes",
                     UMI_STUDIO_PANE_PROCESSES, "processes");
    assert_view_kind(workbench, "studio.tasks",
                     UMI_STUDIO_PANE_TASKS, "tasks");
    assert_view_kind(workbench, "studio.terminal-history",
                     UMI_STUDIO_PANE_TERMINAL_HISTORY, "terminal-history");
    assert(umi_ui_pane_model_find(umi_ui_workbench_panes(workbench),
        UMI_STUDIO_PANE_PROCESSES, &pane) == UMI_STATUS_OK);
    assert(!pane.visible);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}

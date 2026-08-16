/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_application_authority.c
 *
 * PURPOSE:
 *   Verify Studio exposes the Framework portfolio, boundary audit and GTK4
 *   coverage without maintaining private application definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/application_centre.h"
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"

static int64_t integer_property(UmiUiViewModel *view, const char *key)
{
    UmiUiValue value;
    assert(umi_ui_view_model_get_property(view, key, &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER);
    return value.integer_value;
}

int main(void)
{
    UmiStudioApplicationCentreSnapshot snapshot;
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiViewModel *applications = NULL;
    UmiUiViewModel *architecture = NULL;
    UmiUiViewModel *gtk4 = NULL;
    UmiUiValue value;
    UmiDesktopSnapshot desktop_snapshot;
    UmiDesktopShellSnapshot shell_snapshot;

    assert(umi_studio_application_centre_snapshot(NULL, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.item_count == umi_application_portfolio_count());
    assert(snapshot.item_count >= 22U);
    assert(snapshot.reusable_component_count >= 90U);
    assert(snapshot.gtk4_widget_count >= 90U);
    assert(snapshot.architecture_passed);
    assert(snapshot.architecture_error_count == 0U);

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(strcmp(umi_studio_bootstrap_application_name(bootstrap),
                  "Umicom Studio IDE") == 0);
    assert(umi_studio_bootstrap_module_count(bootstrap) == 1U);
    assert(umi_desktop_runtime_snapshot(
        umi_studio_bootstrap_desktop_runtime(bootstrap), &desktop_snapshot) ==
        UMI_STATUS_OK);
    assert(desktop_snapshot.layout_count == 16U);
    assert(desktop_snapshot.tab_count == 16U);
    assert(desktop_snapshot.tab_placement == UMI_DESKTOP_LAYOUT_TABS_BOTTOM);
    assert(strcmp(desktop_snapshot.active_layout_id, "develop") == 0);
    assert(umi_desktop_shell_model_snapshot(
        umi_studio_bootstrap_desktop_shell(bootstrap), &shell_snapshot) ==
        UMI_STATUS_OK);
    assert(shell_snapshot.tab_count == 16U);
    assert(shell_snapshot.monitor_count == 1U);
    assert(strcmp(shell_snapshot.active_layout_name, "Develop") == 0);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(umi_ui_view_factory_create_view(
        umi_ui_workbench_view_factories(workbench), "studio.application-hub",
        UMI_STUDIO_PANE_APPLICATIONS, &applications) == UMI_STATUS_OK);
    assert(integer_property(applications, "applications.total") ==
           (int64_t)snapshot.item_count);
    assert(umi_ui_view_model_get_property(applications, "selected.id", &value) ==
           UMI_STATUS_OK);
    assert(strcmp(value.string_value, "org.umicom.studio") == 0);

    assert(umi_ui_view_factory_create_view(
        umi_ui_workbench_view_factories(workbench), "studio.architecture",
        UMI_STUDIO_PANE_ARCHITECTURE, &architecture) == UMI_STATUS_OK);
    assert(integer_property(architecture, "audit.errors") == 0);
    assert(umi_ui_view_factory_create_view(
        umi_ui_workbench_view_factories(workbench), "studio.gtk4-coverage",
        UMI_STUDIO_PANE_GTK4_COVERAGE, &gtk4) == UMI_STATUS_OK);
    assert(integer_property(gtk4, "gtk4.widgets") >= 90);

    umi_ui_view_model_destroy(gtk4);
    umi_ui_view_model_destroy(architecture);
    umi_ui_view_model_destroy(applications);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

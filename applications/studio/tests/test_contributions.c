/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_contributions.c
 *
 * PURPOSE:
 *   Validate the Studio Framework UI composition and workbench integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/studio/workspace_profiles.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiPaneSnapshot pane;
    UmiUiWorkbench *workbench;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(umi_ui_contribution_model_count(
               umi_ui_workbench_contributions(workbench)) ==
           umi_studio_contribution_definition_count());
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench), UMI_STUDIO_PANE_EXPLORER,
               &pane) == UMI_STATUS_OK);
    assert(pane.visible);

    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_DEBUG_CONSOLE, &pane) == UMI_STATUS_OK);
    assert(pane.placement == UMI_UI_PLACEMENT_BOTTOM);
    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEBUG) ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_DEBUG_CONSOLE, &pane) == UMI_STATUS_OK);
    assert(pane.visible);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_SOURCE_CONTROL) ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench), UMI_STUDIO_PANE_VCS_COMMIT,
               &pane) == UMI_STATUS_OK);
    assert(pane.visible);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_VCS_OPERATIONS, &pane) == UMI_STATUS_OK);
    assert(pane.visible);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_TESTING) ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench), UMI_STUDIO_PANE_TESTING,
               &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_LEFT);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_TEST_FAILURES, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_BOTTOM);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_TEST_COVERAGE, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_RIGHT);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_BUILD) ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_BUILD_DASHBOARD, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_LEFT);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_BUILD_OUTPUT, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_BOTTOM);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_BUILD_ARTIFACTS, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_RIGHT);

    assert(umi_ui_workbench_activate_workspace_profile(
               workbench, UMI_STUDIO_WORKSPACE_PROFILE_TRADING) ==
           UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_TRADING_WATCHLIST, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_LEFT);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_TRADING_ORDER_TICKET, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_RIGHT);
    assert(umi_ui_pane_model_find(
               umi_ui_workbench_panes(workbench),
               UMI_STUDIO_PANE_TRADING_ORDERS, &pane) == UMI_STATUS_OK);
    assert(pane.visible && pane.placement == UMI_UI_PLACEMENT_BOTTOM);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

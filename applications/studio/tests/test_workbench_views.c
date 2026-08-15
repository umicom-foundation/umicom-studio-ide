/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_views.c
 *
 * PURPOSE:
 *   Verify that Studio registers real Framework view factories for the unified
 *   workbench and that the Explorer view can be materialised as a presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/studio/workbench_views.h"
#include "umicom/ui/view_presentation.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiViewPresentation presentation;
    UmiUiPropertySnapshot title;
    UmiUiPropertySnapshot debug_state;
    UmiUiViewModel *run_debug = NULL;
    UmiUiViewModel *debug_watches = NULL;
    UmiUiViewModel *source_control = NULL;
    UmiUiViewModel *source_control_commit = NULL;
    UmiUiViewModel *test_explorer = NULL;
    UmiUiViewModel *test_results = NULL;
    UmiUiViewModel *build_dashboard = NULL;
    UmiUiViewModel *build_graph = NULL;
    UmiUiViewModel *trading_watchlist = NULL;
    UmiUiViewModel *trading_ticket = NULL;
    UmiUiCommandViewAction action;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);

    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(workbench != NULL);
    assert(umi_ui_view_factory_count(
               umi_ui_workbench_view_factories(workbench)) >=
           umi_studio_workbench_view_definition_count());

    assert(umi_ui_view_presentation_build(
               umi_ui_workbench_view_factories(workbench),
               "studio.project-explorer",
               UMI_STUDIO_PANE_EXPLORER,
               &presentation) == UMI_STATUS_OK);

    assert(umi_ui_view_presentation_find_property(
               &presentation, "title", &title) == UMI_STATUS_OK);
    assert(title.value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(title.value.string_value, "Explorer") == 0);

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.run-debug",
               UMI_STUDIO_PANE_RUN_DEBUG,
               &run_debug) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               run_debug, "debug-state", &debug_state.value) == UMI_STATUS_OK);
    assert(debug_state.value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(debug_state.value.string_value, "idle") == 0);
    assert(umi_ui_command_view_action_at(run_debug, 4U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.debug.start") == 0);
    assert(umi_ui_command_view_action_at(run_debug, 11U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.debug.add-breakpoint") == 0);
    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.debug-watches",
               UMI_STUDIO_PANE_DEBUG_WATCHES,
               &debug_watches) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(debug_watches, 0U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.debug.add-watch") == 0);
    umi_ui_view_model_destroy(debug_watches);
    umi_ui_view_model_destroy(run_debug);

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.source-control", UMI_STUDIO_PANE_SOURCE_CONTROL,
               &source_control) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(source_control, 1U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.vcs.filter") == 0);
    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.vcs-commit", UMI_STUDIO_PANE_VCS_COMMIT,
               &source_control_commit) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(
               source_control_commit, 0U, &action) == UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.vcs.set-commit-message") == 0);
    umi_ui_view_model_destroy(source_control_commit);
    umi_ui_view_model_destroy(source_control);

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench), "studio.testing",
               UMI_STUDIO_PANE_TESTING, &test_explorer) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(test_explorer, 3U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.test.run-all") == 0);
    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.test-results", UMI_STUDIO_PANE_TEST_RESULTS,
               &test_results) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(test_results, 1U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.test.clear-results") == 0);
    umi_ui_view_model_destroy(test_results);
    umi_ui_view_model_destroy(test_explorer);

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.build-dashboard", UMI_STUDIO_PANE_BUILD_DASHBOARD,
               &build_dashboard) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(build_dashboard, 6U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.run-all") == 0);
    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.build-graph", UMI_STUDIO_PANE_BUILD_GRAPH,
               &build_graph) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(build_graph, 2U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.run-next") == 0);
    umi_ui_view_model_destroy(build_graph);
    umi_ui_view_model_destroy(build_dashboard);

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.trading-watchlist",
               UMI_STUDIO_PANE_TRADING_WATCHLIST,
               &trading_watchlist) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(trading_watchlist, 0U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.trading.filter-instruments") == 0);
    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.trading-order-ticket",
               UMI_STUDIO_PANE_TRADING_ORDER_TICKET,
               &trading_ticket) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(trading_ticket, 5U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.trading.submit-order") == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(trading_ticket);
    umi_ui_view_model_destroy(trading_watchlist);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

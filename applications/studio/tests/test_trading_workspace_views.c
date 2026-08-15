/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_trading_workspace_views.c
 *
 * PURPOSE:
 *   Verify Studio composes all eight Framework trading panes and activates
 *   their left, right and bottom placements through the Trading profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/studio/workspace_profiles.h"
#include "umicom/ui/command_view.h"

static void verify_view(UmiUiWorkbench *workbench, const char *view_type,
                        const char *pane_id, const char *expected_kind)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue kind;

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench), view_type, pane_id,
               &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &kind) == UMI_STATUS_OK);
    assert(kind.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(kind.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiPaneSnapshot pane;
    UmiUiViewModel *ticket = NULL;
    UmiUiCommandViewAction action;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));

    verify_view(workbench, "studio.trading-dashboard",
                UMI_STUDIO_PANE_TRADING_DASHBOARD, "trading-dashboard");
    verify_view(workbench, "studio.trading-watchlist",
                UMI_STUDIO_PANE_TRADING_WATCHLIST, "trading-watchlist");
    verify_view(workbench, "studio.trading-depth",
                UMI_STUDIO_PANE_TRADING_DEPTH, "trading-depth");
    verify_view(workbench, "studio.trading-chart",
                UMI_STUDIO_PANE_TRADING_CHART, "trading-chart");
    verify_view(workbench, "studio.trading-order-ticket",
                UMI_STUDIO_PANE_TRADING_ORDER_TICKET,
                "trading-order-ticket");
    verify_view(workbench, "studio.trading-orders",
                UMI_STUDIO_PANE_TRADING_ORDERS, "trading-orders");
    verify_view(workbench, "studio.trading-executions",
                UMI_STUDIO_PANE_TRADING_EXECUTIONS, "trading-executions");
    verify_view(workbench, "studio.trading-portfolio-risk",
                UMI_STUDIO_PANE_TRADING_PORTFOLIO_RISK,
                "trading-portfolio-risk");

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.trading-order-ticket",
               UMI_STUDIO_PANE_TRADING_ORDER_TICKET, &ticket) ==
           UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(ticket, 5U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.trading.submit-order") == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(ticket);

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

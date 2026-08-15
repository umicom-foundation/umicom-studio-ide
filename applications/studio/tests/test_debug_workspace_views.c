/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_debug_workspace_views.c
 *
 * PURPOSE:
 *   Verify dedicated Debug panes consume the reusable debugger-workspace state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/studio/debug_workspace_views.h"

int main(void)
{
    UmiStudioDebuggerService *debugger = NULL;
    UmiUiViewModel *watches = NULL;
    UmiUiViewModel *breakpoints = NULL;
    UmiUiViewModel *console = NULL;
    UmiUiCommandViewAction action;
    char watch_id[128];

    assert(umi_studio_debugger_service_create(&debugger) == UMI_STATUS_OK);
    assert(umi_studio_debugger_service_add_watch(
               debugger, "counter + 1", watch_id, sizeof(watch_id)) ==
           UMI_STATUS_OK);
    assert(umi_studio_debugger_service_add_breakpoint(
               debugger, "main.c", 42, 1) == UMI_STATUS_OK);

    assert(umi_studio_debug_watches_view_create(
               "test.watches", debugger, &watches) == UMI_STATUS_OK);
    assert(umi_studio_debug_breakpoints_view_create(
               "test.breakpoints", debugger, &breakpoints) == UMI_STATUS_OK);
    assert(umi_studio_debug_console_view_create(
               "test.console", debugger, &console) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(watches, 0U, &action) ==
           UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(breakpoints, 2U, &action) ==
           UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(console, 0U, &action) ==
           UMI_STATUS_OK);

    umi_ui_view_model_destroy(console);
    umi_ui_view_model_destroy(breakpoints);
    umi_ui_view_model_destroy(watches);
    umi_studio_debugger_service_destroy(debugger);
    return 0;
}

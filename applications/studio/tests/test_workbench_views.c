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
    umi_ui_view_model_destroy(run_debug);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

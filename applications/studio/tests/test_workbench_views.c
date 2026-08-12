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

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

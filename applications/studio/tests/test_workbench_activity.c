/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_activity.c
 *
 * PURPOSE:
 *   Verify activity activation drives the Framework view-container state and
 *   switches the visible Studio primary-sidebar pane without touching product
 *   internals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/studio/ui.h"
#include "umicom/studio/workbench_shell_catalogue.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchSnapshot snapshot;
    UmiUiPaneSnapshot explorer;
    UmiUiPaneSnapshot search;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));

    assert(umi_ui_workbench_activate_activity(workbench,
                                              UMI_STUDIO_ACTIVITY_SEARCH) == UMI_STATUS_OK);
    assert(umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.active_activity, UMI_STUDIO_ACTIVITY_SEARCH) == 0);
    assert(strcmp(snapshot.active_view_container,
                  UMI_STUDIO_CONTAINER_SEARCH) == 0);

    assert(umi_ui_pane_model_find(umi_ui_workbench_panes(workbench),
                                  UMI_STUDIO_PANE_EXPLORER,
                                  &explorer) == UMI_STATUS_OK);
    assert(umi_ui_pane_model_find(umi_ui_workbench_panes(workbench),
                                  UMI_STUDIO_PANE_SEARCH,
                                  &search) == UMI_STATUS_OK);
    assert(!explorer.visible);
    assert(search.visible);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

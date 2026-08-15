/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_shell_catalogue.c
 *
 * PURPOSE:
 *   Verify Batch 23 Studio workbench services and primary-sidebar hosts are
 *   present after normal Framework bootstrap.
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
    UmiUiPaneSnapshot application_hub;
    UmiUiViewContainerSnapshot source_control;
    UmiStatus status;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(workbench != NULL);

    assert(umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.activity_count >= 9U);
    assert(snapshot.view_container_count >= 9U);
    assert(snapshot.keybinding_count >= 12U);
    assert(strcmp(snapshot.active_activity, UMI_STUDIO_ACTIVITY_EXPLORER) == 0);

    assert(umi_ui_pane_model_find(umi_ui_workbench_panes(workbench),
                                  UMI_STUDIO_PANE_APPLICATIONS,
                                  &application_hub) == UMI_STATUS_OK);
    assert(strcmp(application_hub.view_type, "studio.application-hub") == 0);
    assert(umi_ui_view_container_model_find(
               umi_ui_workbench_view_containers(workbench),
               UMI_STUDIO_CONTAINER_SCM, &source_control) == UMI_STATUS_OK);
    assert(source_control.view_count == 8U);
    assert(strcmp(source_control.view_ids[1],
                  UMI_STUDIO_PANE_VCS_COMMIT) == 0);
    assert(strcmp(source_control.view_ids[7],
                  UMI_STUDIO_PANE_VCS_OPERATIONS) == 0);

    status = umi_studio_bootstrap_stop(bootstrap);
    assert(status == UMI_STATUS_OK || status == UMI_STATUS_INVALID_STATE);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

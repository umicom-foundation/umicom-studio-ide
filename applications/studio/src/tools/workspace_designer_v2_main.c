/* Umicom Studio IDE | Native Workspace Designer v2 command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/workspace_commands_v2.h"
#include "umicom/studio/workspace_views_v2.h"
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioProfessionalWorkspaceV2 *workspace = NULL;
    UmiStudioProfessionalWorkspaceSnapshotV2 snapshot;
    UmiStudioWorkspaceViewV2 view;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_professional_workspace_v2_create(umi_studio_bootstrap_services(bootstrap),&workspace);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_SEED);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_view_v2_resolve(workspace,"new-window",&view);
    if (status == UMI_STATUS_OK) status = umi_studio_professional_workspace_v2_snapshot(workspace,&snapshot);
    if (status == UMI_STATUS_OK) (void)printf("Workspace Designer: %zu layouts, %zu windows, %zu groups; active=%s; %s=%zu\n",snapshot.customisation.layouts,snapshot.customisation.available_windows,snapshot.customisation.groups,snapshot.customisation.active_layout_id,view.title,view.item_count);
    umi_studio_professional_workspace_v2_destroy(workspace);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

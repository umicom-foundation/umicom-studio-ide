/* Umicom Studio IDE | Native Workspace Designer v2 command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/workspace_commands.h"
#include "umicom/studio/workspace_views.h"
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioProfessionalWorkspace *workspace = NULL;
    UmiStudioProfessionalWorkspaceSnapshot snapshot;
    UmiStudioWorkspaceView view;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_professional_workspace_create(umi_studio_bootstrap_services(bootstrap),&workspace);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_execute(workspace,UMI_STUDIO_WORKSPACE_COMMAND_SEED);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_execute(workspace,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE);
    if (status == UMI_STATUS_OK) status = umi_studio_workspace_view_resolve(workspace,"new-window",&view);
    if (status == UMI_STATUS_OK) status = umi_studio_professional_workspace_snapshot(workspace,&snapshot);
    if (status == UMI_STATUS_OK) (void)printf("Workspace Designer: %zu layouts, %zu windows, %zu groups; active=%s; %s=%zu\n",snapshot.customisation.layouts,snapshot.customisation.available_windows,snapshot.customisation.groups,snapshot.customisation.active_layout_id,view.title,view.item_count);
    umi_studio_professional_workspace_destroy(workspace);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

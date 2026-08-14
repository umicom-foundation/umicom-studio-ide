/* Umicom Studio IDE | Professional workspace v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/professional_workspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiStudioProfessionalWorkspace {
    UmiStudioServices *services;
    UmiUiWorkspaceCustomisation customisation;
    char active_view[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t revision;
};
UmiStatus umi_studio_professional_workspace_create(UmiStudioServices *services,UmiStudioProfessionalWorkspace **out_workspace)
{
    UmiStudioProfessionalWorkspace *workspace;
    if (services == NULL || out_workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_workspace = NULL;
    workspace = (UmiStudioProfessionalWorkspace *)calloc(1U,sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->services = services;
    umi_ui_workspace_customisation_init(&workspace->customisation);
    (void)snprintf(workspace->active_view,sizeof(workspace->active_view),"overview");
    workspace->revision = 1U;
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}
void umi_studio_professional_workspace_destroy(UmiStudioProfessionalWorkspace *workspace) { free(workspace); }
UmiStatus umi_studio_professional_workspace_activate_view(UmiStudioProfessionalWorkspace *workspace,const char *view_id)
{
    int length;
    if (workspace == NULL || view_id == NULL || view_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = snprintf(workspace->active_view,sizeof(workspace->active_view),"%s",view_id);
    if (length < 0 || (size_t)length >= sizeof(workspace->active_view)) return UMI_STATUS_CAPACITY_EXCEEDED;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_studio_professional_workspace_snapshot(const UmiStudioProfessionalWorkspace *workspace,UmiStudioProfessionalWorkspaceSnapshot *out_snapshot)
{
    if (workspace == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    umi_ui_workspace_customisation_snapshot(&workspace->customisation,&out_snapshot->customisation);
    (void)snprintf(out_snapshot->active_view,sizeof(out_snapshot->active_view),"%s",workspace->active_view);
    out_snapshot->revision = workspace->revision + out_snapshot->customisation.revision;
    return UMI_STATUS_OK;
}
UmiUiWorkspaceCustomisation *umi_studio_professional_workspace_model(UmiStudioProfessionalWorkspace *workspace) { return workspace == NULL ? NULL : &workspace->customisation; }

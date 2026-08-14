/* Umicom Studio IDE | Workspace linked groups v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_groups_v2.h"
static UmiStatus join(UmiUiWindowGroupStoreV2 *groups,const char *group_id,const char *window_id,UmiUiWindowGroupRole role)
{ return umi_ui_window_group_v2_join(groups,group_id,window_id,role); }
UmiStatus umi_studio_workspace_groups_v2_seed(UmiStudioProfessionalWorkspaceV2 *workspace)
{
    UmiUiWorkspaceCustomisationV2 *model = umi_studio_professional_workspace_v2_model(workspace);
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_v2_define(&model->groups,"project-blue","workspace.group.blue",UMI_UI_WINDOW_CONTEXT_PROJECT); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"project-blue","project-explorer",UMI_UI_WINDOW_GROUP_SOURCE); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"project-blue","editor-main",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"project-blue","authorengine",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    status = umi_ui_window_group_v2_define(&model->groups,"run-green","workspace.group.green",UMI_UI_WINDOW_CONTEXT_RUN); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"run-green","metrics",UMI_UI_WINDOW_GROUP_SOURCE); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"run-green","traces",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"run-green","profiler",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"run-green","health",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    status = umi_ui_window_group_v2_define(&model->groups,"compare-orange","workspace.group.orange",UMI_UI_WINDOW_CONTEXT_FILE); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"compare-orange","compare-original",UMI_UI_WINDOW_GROUP_SOURCE); if (status != UMI_STATUS_OK) return status;
    status = join(&model->groups,"compare-orange","compare-modified",UMI_UI_WINDOW_GROUP_DESTINATION); if (status != UMI_STATUS_OK) return status;
    return join(&model->groups,"compare-orange","compare-results",UMI_UI_WINDOW_GROUP_DESTINATION);
}

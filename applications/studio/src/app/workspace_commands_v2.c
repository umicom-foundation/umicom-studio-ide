/* Umicom Studio IDE | Workspace designer commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_commands_v2.h"
#include "umicom/studio/workspace_catalogue_v2.h"
#include "umicom/studio/workspace_groups_v2.h"
#include "umicom/studio/workspace_layouts_v2.h"
UmiStatus umi_studio_workspace_seed_v2(UmiStudioProfessionalWorkspaceV2 *workspace)
{
    UmiStatus status;
    status = umi_studio_workspace_catalogue_v2_seed(workspace); if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_layouts_v2_seed(workspace); if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_groups_v2_seed(workspace); if (status != UMI_STATUS_OK) return status;
    return umi_studio_workspace_theme_v2_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_DARK,UMI_UI_DENSITY_COMFORTABLE,1.0);
}
UmiStatus umi_studio_workspace_execute_v2(UmiStudioProfessionalWorkspaceV2 *workspace,UmiStudioWorkspaceCommandV2 command)
{
    UmiUiWorkspaceCustomisationV2 *model = umi_studio_professional_workspace_v2_model(workspace);
    UmiUiWorkspaceLayoutV3 *active;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (command) {
        case UMI_STUDIO_WORKSPACE_COMMAND_SEED: return umi_studio_workspace_seed_v2(workspace);
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_DEVELOP: return umi_ui_workspace_customisation_v2_activate(model,"develop");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_OPERATIONS: return umi_ui_workspace_customisation_v2_activate(model,"operations");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE: return umi_ui_workspace_customisation_v2_activate(model,"compare");
        case UMI_STUDIO_WORKSPACE_COMMAND_UNLOCK:
        case UMI_STUDIO_WORKSPACE_COMMAND_LOCK:
            active = umi_ui_workspace_customisation_v2_active(model);
            return active == NULL ? UMI_STATUS_NOT_FOUND : umi_ui_workspace_layout_v3_set_locked(active,command == UMI_STUDIO_WORKSPACE_COMMAND_LOCK);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_LIGHT: return umi_studio_workspace_theme_v2_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_LIGHT,UMI_UI_DENSITY_COMFORTABLE,1.0);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_DARK: return umi_studio_workspace_theme_v2_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_DARK,UMI_UI_DENSITY_COMFORTABLE,1.0);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_HIGH_CONTRAST: return umi_studio_workspace_theme_v2_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_HIGH_CONTRAST,UMI_UI_DENSITY_COMFORTABLE,1.1);
        default: return UMI_STATUS_NOT_IMPLEMENTED;
    }
}

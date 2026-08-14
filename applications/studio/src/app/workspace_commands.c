/* Umicom Studio IDE | Workspace designer commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_commands.h"
#include "umicom/studio/workspace_catalogue.h"
#include "umicom/studio/workspace_groups.h"
#include "umicom/studio/workspace_layouts.h"
UmiStatus umi_studio_workspace_seed(UmiStudioProfessionalWorkspace *workspace)
{
    UmiStatus status;
    status = umi_studio_workspace_catalogue_seed(workspace); if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_layouts_seed(workspace); if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_groups_seed(workspace); if (status != UMI_STATUS_OK) return status;
    return umi_studio_workspace_theme_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_DARK,UMI_UI_DENSITY_COMPACT,1.0);
}
UmiStatus umi_studio_workspace_execute(UmiStudioProfessionalWorkspace *workspace,UmiStudioWorkspaceCommand command)
{
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    UmiUiWorkspaceLayout *active;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (command) {
        case UMI_STUDIO_WORKSPACE_COMMAND_SEED: return umi_studio_workspace_seed(workspace);
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_DEVELOP: return umi_ui_workspace_customisation_activate(model,"develop");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_FOCUS: return umi_ui_workspace_customisation_activate(model,"focus");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_DEBUG: return umi_ui_workspace_customisation_activate(model,"debug");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_OPERATIONS: return umi_ui_workspace_customisation_activate(model,"operations");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_TRADING: return umi_ui_workspace_customisation_activate(model,"trading");
        case UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE: return umi_ui_workspace_customisation_activate(model,"compare");
        case UMI_STUDIO_WORKSPACE_COMMAND_UNLOCK:
        case UMI_STUDIO_WORKSPACE_COMMAND_LOCK:
            active = umi_ui_workspace_customisation_active(model);
            return active == NULL ? UMI_STATUS_NOT_FOUND : umi_ui_workspace_layout_set_locked(active,command == UMI_STUDIO_WORKSPACE_COMMAND_LOCK);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_LIGHT: return umi_studio_workspace_theme_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_LIGHT,UMI_UI_DENSITY_COMFORTABLE,1.0);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_DARK: return umi_studio_workspace_theme_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_DARK,UMI_UI_DENSITY_COMFORTABLE,1.0);
        case UMI_STUDIO_WORKSPACE_COMMAND_THEME_HIGH_CONTRAST: return umi_studio_workspace_theme_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_HIGH_CONTRAST,UMI_UI_DENSITY_COMFORTABLE,1.1);
        default: return UMI_STATUS_NOT_IMPLEMENTED;
    }
}

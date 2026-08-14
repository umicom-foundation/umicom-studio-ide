/* Umicom Studio IDE | Workspace designer commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_COMMANDS_V2_H
#define UMICOM_STUDIO_WORKSPACE_COMMANDS_V2_H
#include "umicom/studio/workspace_themes_v2.h"
typedef enum UmiStudioWorkspaceCommandV2 {
    UMI_STUDIO_WORKSPACE_COMMAND_SEED = 1,
    UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_DEVELOP,
    UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_OPERATIONS,
    UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE,
    UMI_STUDIO_WORKSPACE_COMMAND_UNLOCK,
    UMI_STUDIO_WORKSPACE_COMMAND_LOCK,
    UMI_STUDIO_WORKSPACE_COMMAND_THEME_LIGHT,
    UMI_STUDIO_WORKSPACE_COMMAND_THEME_DARK,
    UMI_STUDIO_WORKSPACE_COMMAND_THEME_HIGH_CONTRAST
} UmiStudioWorkspaceCommandV2;
UmiStatus umi_studio_workspace_seed_v2(UmiStudioProfessionalWorkspaceV2 *workspace);
UmiStatus umi_studio_workspace_execute_v2(UmiStudioProfessionalWorkspaceV2 *workspace,UmiStudioWorkspaceCommandV2 command);
#endif

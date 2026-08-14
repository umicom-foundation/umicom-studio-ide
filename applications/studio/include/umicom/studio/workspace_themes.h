/* Umicom Studio IDE | Workspace themes v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_THEMES_H
#define UMICOM_STUDIO_WORKSPACE_THEMES_H
#include "umicom/studio/professional_workspace.h"
typedef enum UmiStudioWorkspaceTheme {
    UMI_STUDIO_WORKSPACE_THEME_SYSTEM = 1,
    UMI_STUDIO_WORKSPACE_THEME_LIGHT,
    UMI_STUDIO_WORKSPACE_THEME_DARK,
    UMI_STUDIO_WORKSPACE_THEME_HIGH_CONTRAST
} UmiStudioWorkspaceTheme;
UmiStatus umi_studio_workspace_theme_apply(UmiStudioProfessionalWorkspace *workspace,UmiStudioWorkspaceTheme theme,UmiUiDensity density,double font_scale);
#endif

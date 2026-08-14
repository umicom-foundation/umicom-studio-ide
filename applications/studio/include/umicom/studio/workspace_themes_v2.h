/* Umicom Studio IDE | Workspace themes v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_THEMES_V2_H
#define UMICOM_STUDIO_WORKSPACE_THEMES_V2_H
#include "umicom/studio/professional_workspace_v2.h"
typedef enum UmiStudioWorkspaceThemeV2 {
    UMI_STUDIO_WORKSPACE_THEME_SYSTEM = 1,
    UMI_STUDIO_WORKSPACE_THEME_LIGHT,
    UMI_STUDIO_WORKSPACE_THEME_DARK,
    UMI_STUDIO_WORKSPACE_THEME_HIGH_CONTRAST
} UmiStudioWorkspaceThemeV2;
UmiStatus umi_studio_workspace_theme_v2_apply(UmiStudioProfessionalWorkspaceV2 *workspace,UmiStudioWorkspaceThemeV2 theme,UmiUiDensityV2 density,double font_scale);
#endif

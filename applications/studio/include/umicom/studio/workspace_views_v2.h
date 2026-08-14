/* Umicom Studio IDE | Workspace designer views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_VIEWS_V2_H
#define UMICOM_STUDIO_WORKSPACE_VIEWS_V2_H
#include "umicom/studio/professional_workspace_v2.h"
typedef enum UmiStudioWorkspaceViewKindV2 {
    UMI_STUDIO_WORKSPACE_VIEW_OVERVIEW = 1,
    UMI_STUDIO_WORKSPACE_VIEW_LAYOUTS,
    UMI_STUDIO_WORKSPACE_VIEW_NEW_WINDOW,
    UMI_STUDIO_WORKSPACE_VIEW_GROUPS,
    UMI_STUDIO_WORKSPACE_VIEW_THEMES,
    UMI_STUDIO_WORKSPACE_VIEW_PERSISTENCE
} UmiStudioWorkspaceViewKindV2;
typedef struct UmiStudioWorkspaceViewV2 {
    UmiStudioWorkspaceViewKindV2 kind;
    char view_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char summary[384U];
    size_t item_count;
    bool available;
} UmiStudioWorkspaceViewV2;
size_t umi_studio_workspace_view_v2_count(void);
UmiStatus umi_studio_workspace_view_v2_resolve(const UmiStudioProfessionalWorkspaceV2 *workspace,const char *view_id,UmiStudioWorkspaceViewV2 *out_view);
#endif

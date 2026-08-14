/* Umicom Studio IDE | Workspace designer views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_VIEWS_H
#define UMICOM_STUDIO_WORKSPACE_VIEWS_H
#include "umicom/studio/professional_workspace.h"
typedef enum UmiStudioWorkspaceViewKind {
    UMI_STUDIO_WORKSPACE_VIEW_OVERVIEW = 1,
    UMI_STUDIO_WORKSPACE_VIEW_LAYOUTS,
    UMI_STUDIO_WORKSPACE_VIEW_NEW_WINDOW,
    UMI_STUDIO_WORKSPACE_VIEW_GROUPS,
    UMI_STUDIO_WORKSPACE_VIEW_THEMES,
    UMI_STUDIO_WORKSPACE_VIEW_PERSISTENCE
} UmiStudioWorkspaceViewKind;
typedef struct UmiStudioWorkspaceView {
    UmiStudioWorkspaceViewKind kind;
    char view_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char title[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    char summary[384U];
    size_t item_count;
    bool available;
} UmiStudioWorkspaceView;
size_t umi_studio_workspace_view_count(void);
UmiStatus umi_studio_workspace_view_resolve(const UmiStudioProfessionalWorkspace *workspace,const char *view_id,UmiStudioWorkspaceView *out_view);
#endif

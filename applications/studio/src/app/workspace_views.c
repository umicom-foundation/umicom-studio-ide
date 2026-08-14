/* Umicom Studio IDE | Workspace designer views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_views.h"
#include <stdio.h>
#include <string.h>
typedef struct ViewDefinition { UmiStudioWorkspaceViewKind kind; const char *id; const char *title; const char *summary; } ViewDefinition;
static const ViewDefinition VIEWS[] = {
    {UMI_STUDIO_WORKSPACE_VIEW_OVERVIEW,"overview","Workspace Overview","Active layout, edit lock, theme and customisation summary"},
    {UMI_STUDIO_WORKSPACE_VIEW_LAYOUTS,"layouts","Layouts","Create, switch, clone, organise and lock layout tabs"},
    {UMI_STUDIO_WORKSPACE_VIEW_NEW_WINDOW,"new-window","New Window","Search the catalogue and add windows or subwindows"},
    {UMI_STUDIO_WORKSPACE_VIEW_GROUPS,"groups","Linked Windows","Route project, file, run or trading context across grouped panels"},
    {UMI_STUDIO_WORKSPACE_VIEW_THEMES,"themes","Appearance","Theme, colour tokens, density, font scale and accessibility"},
    {UMI_STUDIO_WORKSPACE_VIEW_PERSISTENCE,"persistence","Save and Restore","Versioned local file or Data Server persistence with recovery"}
};
size_t umi_studio_workspace_view_count(void) { return sizeof(VIEWS) / sizeof(VIEWS[0]); }
UmiStatus umi_studio_workspace_view_resolve(const UmiStudioProfessionalWorkspace *workspace,const char *view_id,UmiStudioWorkspaceView *out_view)
{
    UmiStudioProfessionalWorkspaceSnapshot snapshot;
    size_t index;
    size_t item_count;
    if (workspace == NULL || view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_studio_professional_workspace_snapshot(workspace,&snapshot) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < umi_studio_workspace_view_count(); ++index) {
        if (strcmp(VIEWS[index].id,view_id) != 0) continue;
        switch (VIEWS[index].kind) {
            case UMI_STUDIO_WORKSPACE_VIEW_LAYOUTS: item_count = snapshot.customisation.layouts; break;
            case UMI_STUDIO_WORKSPACE_VIEW_NEW_WINDOW: item_count = snapshot.customisation.available_windows; break;
            case UMI_STUDIO_WORKSPACE_VIEW_GROUPS: item_count = snapshot.customisation.groups; break;
            case UMI_STUDIO_WORKSPACE_VIEW_THEMES: item_count = 4U; break;
            case UMI_STUDIO_WORKSPACE_VIEW_PERSISTENCE: item_count = snapshot.customisation.layouts; break;
            case UMI_STUDIO_WORKSPACE_VIEW_OVERVIEW: item_count = snapshot.customisation.layouts + snapshot.customisation.available_windows + snapshot.customisation.groups; break;
            default: return UMI_STATUS_NOT_IMPLEMENTED;
        }
        (void)memset(out_view,0,sizeof(*out_view));
        out_view->kind = VIEWS[index].kind;
        (void)snprintf(out_view->view_id,sizeof(out_view->view_id),"%s",VIEWS[index].id);
        (void)snprintf(out_view->title,sizeof(out_view->title),"%s",VIEWS[index].title);
        (void)snprintf(out_view->summary,sizeof(out_view->summary),"%s",VIEWS[index].summary);
        out_view->item_count = item_count;
        out_view->available = true;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

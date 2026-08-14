/* Umicom Studio IDE | Professional workspace v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_PROFESSIONAL_WORKSPACE_H
#define UMICOM_STUDIO_PROFESSIONAL_WORKSPACE_H
#include "umicom/studio/services.h"
#include "umicom/ui/professional_workspace.h"
typedef struct UmiStudioProfessionalWorkspace UmiStudioProfessionalWorkspace;
typedef struct UmiStudioProfessionalWorkspaceSnapshot {
    UmiUiWorkspaceCustomisationSnapshot customisation;
    char active_view[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t revision;
} UmiStudioProfessionalWorkspaceSnapshot;
UmiStatus umi_studio_professional_workspace_create(UmiStudioServices *services,UmiStudioProfessionalWorkspace **out_workspace);
void umi_studio_professional_workspace_destroy(UmiStudioProfessionalWorkspace *workspace);
UmiStatus umi_studio_professional_workspace_activate_view(UmiStudioProfessionalWorkspace *workspace,const char *view_id);
UmiStatus umi_studio_professional_workspace_snapshot(const UmiStudioProfessionalWorkspace *workspace,UmiStudioProfessionalWorkspaceSnapshot *out_snapshot);
UmiUiWorkspaceCustomisation *umi_studio_professional_workspace_model(UmiStudioProfessionalWorkspace *workspace);
#endif

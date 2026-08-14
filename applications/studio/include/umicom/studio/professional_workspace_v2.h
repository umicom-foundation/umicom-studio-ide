/* Umicom Studio IDE | Professional workspace v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_PROFESSIONAL_WORKSPACE_V2_H
#define UMICOM_STUDIO_PROFESSIONAL_WORKSPACE_V2_H
#include "umicom/studio/services.h"
#include "umicom/ui/professional_workspace.h"
typedef struct UmiStudioProfessionalWorkspaceV2 UmiStudioProfessionalWorkspaceV2;
typedef struct UmiStudioProfessionalWorkspaceSnapshotV2 {
    UmiUiWorkspaceCustomisationSnapshotV2 customisation;
    char active_view[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t revision;
} UmiStudioProfessionalWorkspaceSnapshotV2;
UmiStatus umi_studio_professional_workspace_v2_create(UmiStudioServices *services,UmiStudioProfessionalWorkspaceV2 **out_workspace);
void umi_studio_professional_workspace_v2_destroy(UmiStudioProfessionalWorkspaceV2 *workspace);
UmiStatus umi_studio_professional_workspace_v2_activate_view(UmiStudioProfessionalWorkspaceV2 *workspace,const char *view_id);
UmiStatus umi_studio_professional_workspace_v2_snapshot(const UmiStudioProfessionalWorkspaceV2 *workspace,UmiStudioProfessionalWorkspaceSnapshotV2 *out_snapshot);
UmiUiWorkspaceCustomisationV2 *umi_studio_professional_workspace_v2_model(UmiStudioProfessionalWorkspaceV2 *workspace);
#endif

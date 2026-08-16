/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_groups.c
 *
 * PURPOSE:
 *   Adapt Framework-owned desktop context links to Studio. All group policy,
 *   roles and membership remain reusable Umicom Framework definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace_groups.h"

#include "umicom/desktop/desktop.h"

UmiStatus umi_studio_workspace_groups_seed(
    UmiStudioProfessionalWorkspace *workspace)
{
    UmiUiWorkspaceCustomisation *model =
        umi_studio_professional_workspace_model(workspace);
    UmiApplicationContextHub *context_hub = NULL;
    UmiDesktopRuntime *desktop = NULL;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_context_hub_create(&context_hub);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_runtime_create(context_hub, &desktop);
    if (status == UMI_STATUS_OK) status = umi_desktop_runtime_seed(desktop);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_seed_workspace_groups(
            &model->groups, umi_desktop_runtime_context_links(desktop));
    umi_desktop_runtime_destroy(desktop);
    umi_application_context_hub_destroy(context_hub);
    return status;
}

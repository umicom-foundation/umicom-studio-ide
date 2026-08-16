/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_layouts.c
 *
 * PURPOSE:
 *   Adapt the Framework-owned professional desktop layout catalogue to the
 *   Studio semantic workspace model. Studio contains no private layout logic;
 *   every definition and reusable behaviour comes from Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace_layouts.h"

#include "umicom/desktop/desktop.h"

UmiStatus umi_studio_workspace_layouts_seed(
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
        status = umi_desktop_seed_workspace_customisation(
            model, umi_desktop_runtime_layouts(desktop),
            umi_desktop_runtime_monitors(desktop), "studio");
    umi_desktop_runtime_destroy(desktop);
    umi_application_context_hub_destroy(context_hub);
    return status;
}

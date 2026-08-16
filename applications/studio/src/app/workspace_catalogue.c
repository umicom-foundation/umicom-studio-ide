/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_catalogue.c
 *
 * PURPOSE:
 *   Populate Studio from the Framework-owned reusable component and window
 *   catalogue. Studio does not maintain private component definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace_catalogue.h"

#include "umicom/desktop/ui_bridge.h"

UmiStatus umi_studio_workspace_catalogue_seed(
    UmiStudioProfessionalWorkspace *workspace)
{
    UmiUiWorkspaceCustomisation *model =
        umi_studio_professional_workspace_model(workspace);
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_desktop_seed_window_catalogue(&model->windows);
}

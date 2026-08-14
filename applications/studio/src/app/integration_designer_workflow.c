/* Umicom Studio IDE | Integration Designer workflow canvas | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_workflow.h"

UmiStatus umi_studio_integration_designer_add_workflow(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationWorkflow *workflow)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_designer_fabric_add_workflow(&centre->fabric,workflow);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiStatus umi_studio_integration_designer_validate_workflow(const UmiStudioIntegrationDesignerCentre *centre,const char *workflow_id,UmiIntegrationDesignerValidation *validation)
{
    const UmiIntegrationWorkflow *workflow;
    if (centre == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workflow = umi_integration_designer_fabric_find_workflow(&centre->fabric,workflow_id);
    return workflow == NULL ? UMI_STATUS_NOT_FOUND : umi_integration_workflow_validate(workflow,validation);
}

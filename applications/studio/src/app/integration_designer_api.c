/* Umicom Studio IDE | Integration Designer API explorer | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_api.h"
#include <string.h>

UmiStatus umi_studio_integration_designer_add_contract(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationApiContract *contract)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (contract == NULL || umi_integration_connection_catalogue_find(&centre->fabric.connections,contract->profile_id) == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_integration_designer_fabric_add_contract(&centre->fabric,contract);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

const UmiIntegrationApiContract *umi_studio_integration_designer_find_contract(const UmiStudioIntegrationDesignerCentre *centre,const char *contract_id)
{
    size_t index;
    if (centre == NULL || contract_id == NULL) return NULL;
    for (index = 0U; index < centre->fabric.contract_count; ++index) if (strcmp(centre->fabric.contracts[index].id,contract_id) == 0) return &centre->fabric.contracts[index];
    return NULL;
}

UmiStatus umi_studio_integration_designer_plan_request(const UmiStudioIntegrationDesignerCentre *centre,const char *contract_id,const char *operation_id,const char *payload,const char *correlation_id,UmiIntegrationRequestPlan *out_plan)
{
    const UmiIntegrationApiContract *contract = umi_studio_integration_designer_find_contract(centre,contract_id);
    const UmiIntegrationApiOperation *operation;
    const UmiIntegrationConnectionProfile *profile;
    if (contract == NULL) return UMI_STATUS_NOT_FOUND;
    operation = umi_integration_api_contract_find_operation(contract,operation_id);
    if (operation == NULL) return UMI_STATUS_NOT_FOUND;
    profile = umi_integration_connection_catalogue_find(&centre->fabric.connections,contract->profile_id);
    if (profile == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_integration_request_plan_build(profile,operation,payload,correlation_id,out_plan);
}

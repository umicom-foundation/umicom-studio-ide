/* Umicom Studio IDE | Integration Designer mapping | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_mapping.h"
#include <string.h>

UmiStatus umi_studio_integration_designer_add_mapping(UmiStudioIntegrationDesignerCentre *centre,const UmiIntegrationFieldMapping *mapping)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_designer_fabric_add_mapping(&centre->fabric,mapping);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

const UmiIntegrationFieldMapping *umi_studio_integration_designer_find_mapping(const UmiStudioIntegrationDesignerCentre *centre,const char *mapping_id)
{
    size_t index;
    if (centre == NULL || mapping_id == NULL) return NULL;
    for (index = 0U; index < centre->fabric.mapping_count; ++index) if (strcmp(centre->fabric.mappings[index].id,mapping_id) == 0) return &centre->fabric.mappings[index];
    return NULL;
}

UmiStatus umi_studio_integration_designer_preview_mapping(const UmiStudioIntegrationDesignerCentre *centre,const char *mapping_id,const UmiIntegrationDesignerRecord *source,UmiIntegrationDesignerRecord *target,UmiIntegrationDesignerValidation *validation)
{
    const UmiIntegrationFieldMapping *mapping = umi_studio_integration_designer_find_mapping(centre,mapping_id);
    return mapping == NULL ? UMI_STATUS_NOT_FOUND : umi_integration_field_mapping_apply(mapping,source,target,validation);
}

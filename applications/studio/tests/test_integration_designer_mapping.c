/* Umicom Studio IDE Tests | Mapping preview | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "integration_designer_fixture.h"
#include "umicom/studio/integration_designer_mapping.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_seeded_integration_designer();
    UmiIntegrationDesignerRecord source = {0};
    UmiIntegrationDesignerRecord target = {0};
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_designer_record_set(&source,"order_id","O-54") == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_preview_mapping(centre,"order-map",&source,&target,&validation) == UMI_STATUS_OK);
    assert(strcmp(umi_integration_designer_record_get(&target,"orderId"),"O-54") == 0);
    assert(umi_studio_integration_designer_find_mapping(centre,"order-map") != NULL);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

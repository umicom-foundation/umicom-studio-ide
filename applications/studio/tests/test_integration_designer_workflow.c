/* Umicom Studio IDE Tests | Workflow canvas | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
#include "umicom/studio/integration_designer_workflow.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_seeded_integration_designer();
    UmiIntegrationDesignerValidation validation = {0};
    const UmiIntegrationWorkflow *workflow = umi_integration_designer_fabric_find_workflow(&centre->fabric,"order-sync");
    assert(workflow != NULL && workflow->node_count == 3U && workflow->edge_count == 2U);
    assert(umi_studio_integration_designer_validate_workflow(centre,"order-sync",&validation) == UMI_STATUS_OK);
    assert(validation.count == 0U);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

/* Umicom Studio IDE Tests | Views and commands | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "integration_designer_fixture.h"
#include "umicom/studio/integration_designer_views.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_integration_designer_centre();
    UmiStudioIntegrationDesignerView view;
    UmiIntegrationSimulation simulation;
    assert(umi_studio_integration_designer_execute(centre,UMI_STUDIO_INTEGRATION_COMMAND_SEED_SAMPLE,NULL) == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_execute(centre,UMI_STUDIO_INTEGRATION_COMMAND_VALIDATE,NULL) == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_execute(centre,UMI_STUDIO_INTEGRATION_COMMAND_SIMULATE,&simulation) == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_view_build(centre,UMI_STUDIO_INTEGRATION_VIEW_MONITORING,&view) == UMI_STATUS_OK);
    assert(strcmp(view.id,"monitoring") == 0 && view.item_count == 1U);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

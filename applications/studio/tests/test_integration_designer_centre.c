/* Umicom Studio IDE Tests | Integration Designer centre | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_integration_designer_centre();
    UmiStudioIntegrationDesignerSnapshot snapshot;
    assert(umi_studio_integration_designer_activate(centre,"workflow") == UMI_STATUS_OK);
    assert(umi_studio_integration_designer_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.active_view,"workflow") == 0 && snapshot.fabric.revision == 1U);
    assert(umi_studio_integration_designer_fabric(centre) != NULL);
    umi_studio_integration_designer_destroy(centre);
    return 0;
}

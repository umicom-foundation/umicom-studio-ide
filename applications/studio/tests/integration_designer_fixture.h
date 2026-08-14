/* Umicom Studio IDE Tests | Integration Designer fixture | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_TEST_INTEGRATION_DESIGNER_FIXTURE_H
#define UMICOM_STUDIO_TEST_INTEGRATION_DESIGNER_FIXTURE_H
#include <assert.h>
#include "umicom/studio/integration_designer_commands.h"
static inline UmiStudioIntegrationDesignerCentre *umi_test_integration_designer_centre(void)
{
    static int services_token;
    UmiStudioIntegrationDesignerCentre *centre = NULL;
    assert(umi_studio_integration_designer_create((UmiStudioServices *)(void *)&services_token,&centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioIntegrationDesignerCentre *umi_test_seeded_integration_designer(void)
{
    UmiStudioIntegrationDesignerCentre *centre = umi_test_integration_designer_centre();
    assert(umi_studio_integration_designer_seed_sample(centre) == UMI_STATUS_OK);
    return centre;
}
#endif

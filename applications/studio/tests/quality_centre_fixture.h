/* Umicom Studio IDE Tests | Quality Centre fixture | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_TEST_QUALITY_CENTRE_FIXTURE_H
#define UMICOM_STUDIO_TEST_QUALITY_CENTRE_FIXTURE_H
#include <assert.h>
#include "umicom/studio/quality_commands.h"
static inline UmiStudioQualityCentre *umi_test_quality_centre(void)
{
    static int services_token;
    UmiStudioQualityCentre *centre = NULL;
    assert(umi_studio_quality_centre_create((UmiStudioServices *)(void *)&services_token,&centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioQualityCentre *umi_test_seeded_quality_centre(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    assert(umi_studio_quality_seed_evidence(centre) == UMI_STATUS_OK);
    return centre;
}
#endif

/* Umicom Studio IDE Tests | Evidence and navigation v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_evidence.h"
#include "umicom/studio/quality_navigation.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiStudioQualityNavigationTarget target;
    assert(umi_studio_quality_record_check(centre,"build.fail",UMI_CODEGUARD_EVIDENCE_BUILD,false,1U,0U,"src/main.c",24U,"Build failed","Open compiler diagnostic") == UMI_STATUS_OK);
    assert(umi_studio_quality_navigation_resolve(centre,"build.fail",&target) == UMI_STATUS_OK);
    assert(strcmp(target.path,"src/main.c") == 0 && target.line == 24U);
    assert(umi_studio_quality_record_architecture_dependency(centre,"applications/studio/src/app.c","framework/src/private.h") == UMI_STATUS_OK);
    assert(centre->session.evidence.count == 2U);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

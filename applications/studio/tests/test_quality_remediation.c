/* Umicom Studio IDE Tests | Remediation guidance v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_evidence.h"
#include "umicom/studio/quality_remediation.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    assert(umi_studio_quality_record_check(centre,"unsafe.copy",UMI_CODEGUARD_EVIDENCE_RULE,false,1U,0U,"src/copy.c",9U,"Unsafe copy","Use capacity-aware copy") == UMI_STATUS_OK);
    assert(umi_studio_quality_remediation_refresh(centre) == UMI_STATUS_OK);
    assert(centre->session.remediation.count == 1U);
    assert(strcmp(umi_studio_quality_remediation_at(centre,0U)->instruction,"Use capacity-aware copy") == 0);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

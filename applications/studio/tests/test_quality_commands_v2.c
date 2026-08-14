/* Umicom Studio IDE Tests | Quality Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "quality_centre_fixture.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiCodeGuardQualityEvaluation evaluation;
    assert(umi_studio_quality_execute(centre,UMI_STUDIO_QUALITY_COMMAND_SEED_EVIDENCE,NULL) == UMI_STATUS_OK);
    assert(umi_studio_quality_execute(centre,UMI_STUDIO_QUALITY_COMMAND_EVALUATE,&evaluation) == UMI_STATUS_OK);
    assert(evaluation.decision == UMI_CODEGUARD_QUALITY_WARN && evaluation.pending_duplicate_reviews == 1U);
    assert(umi_studio_quality_execute(centre,UMI_STUDIO_QUALITY_COMMAND_REFRESH_REMEDIATION,NULL) == UMI_STATUS_OK);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

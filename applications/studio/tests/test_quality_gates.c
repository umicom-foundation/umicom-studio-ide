/* Umicom Studio IDE Tests | Combined quality gates v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_evidence.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiCodeGuardQualityEvaluation evaluation;
    assert(umi_studio_quality_record_check(centre,"tests.fail",UMI_CODEGUARD_EVIDENCE_TEST,false,1U,0U,"tests/test.c",1U,"Test failed","Inspect assertion") == UMI_STATUS_OK);
    assert(umi_studio_quality_evaluate(centre,&evaluation) == UMI_STATUS_OK);
    assert(evaluation.decision == UMI_CODEGUARD_QUALITY_FAIL && evaluation.failures == 1U);
    centre->session.policy.maximum_test_failures = 1U;
    assert(umi_studio_quality_evaluate(centre,&evaluation) == UMI_STATUS_OK);
    assert(evaluation.decision == UMI_CODEGUARD_QUALITY_PASS);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

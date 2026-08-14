/* Umicom Studio IDE Tests | API compatibility v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_compatibility.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiCodeGuardApiBaseline expected;
    UmiCodeGuardApiBaseline current;
    assert(umi_codeguard_api_baseline_init(&expected,"sdk",1U,0U) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_init(&current,"sdk",1U,1U) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&expected,"umi_open",7U,false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&current,"umi_open",8U,false) == UMI_STATUS_OK);
    assert(umi_studio_quality_compare_api(centre,&expected,&current) == UMI_STATUS_OK);
    assert(umi_studio_quality_api_change_at(centre,0U)->level == UMI_CODEGUARD_BREAKING);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

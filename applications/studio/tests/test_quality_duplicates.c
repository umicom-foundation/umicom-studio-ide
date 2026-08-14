/* Umicom Studio IDE Tests | Duplicate review v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_duplicates.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiCodeGuardDuplicateFile first = {"src/a.c",44U,55U,100U};
    UmiCodeGuardDuplicateFile second = {"src/b.c",44U,55U,100U};
    assert(umi_studio_quality_duplicate_add(centre,&first,&second) == UMI_STATUS_OK);
    assert(umi_studio_quality_duplicate_at(centre,0U)->disposition == UMI_CODEGUARD_CANDIDATE_REVIEW);
    assert(umi_studio_quality_duplicate_decide(centre,0U,UMI_CODEGUARD_CANDIDATE_KEEP_SEPARATE) == UMI_STATUS_OK);
    assert(!umi_studio_quality_duplicate_at(centre,0U)->automatic_action_permitted);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

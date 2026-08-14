/* Umicom Studio IDE Tests | Quality Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "quality_centre_fixture.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_quality_centre();
    UmiStudioQualitySnapshot snapshot;
    assert(umi_studio_quality_centre_activate(centre,"evidence") == UMI_STATUS_OK);
    assert(umi_studio_quality_centre_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.active_view,"evidence") == 0 && snapshot.evidence == 0U && snapshot.revision == 2U);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

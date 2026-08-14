/* Umicom Studio IDE Tests | Quality Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "quality_centre_fixture.h"
#include "umicom/studio/quality_views.h"
int main(void)
{
    UmiStudioQualityCentre *centre = umi_test_seeded_quality_centre();
    UmiStudioQualityView view;
    assert(umi_studio_quality_view_build(centre,UMI_STUDIO_QUALITY_VIEW_DUPLICATES,&view) == UMI_STATUS_OK);
    assert(strcmp(view.id,"duplicates") == 0 && view.item_count == 1U);
    assert(strstr(view.summary,"Duplicate Review") != NULL);
    umi_studio_quality_centre_destroy(centre);
    return 0;
}

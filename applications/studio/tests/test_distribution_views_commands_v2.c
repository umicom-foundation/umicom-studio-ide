/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_distribution_views_commands_v2.c
 *
 * PURPOSE:
 *   Verify Distribution Centre view discovery and command enablement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/distribution_commands_v2.h"
#include "umicom/studio/distribution_views_v2.h"

int main(void)
{
    UmiStudioDistributionViewsV2 views;
    const UmiStudioDistributionViewV2 *view;
    umi_studio_distribution_views_v2_init(&views);
    assert(views.count == 8U);
    view = umi_studio_distribution_views_v2_find(&views, "installers");
    assert(view != NULL);
    assert(strcmp(view->title, "Installer Centre") == 0);
    assert(!umi_studio_distribution_command_v2_enabled(
        UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE_V2, 0, 0));
    assert(umi_studio_distribution_command_v2_enabled(
        UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE_V2, 1, 0));
    assert(umi_studio_distribution_command_v2_enabled(
        UMI_STUDIO_DISTRIBUTION_ROLLBACK_V2, 0, 1));
    return 0;
}

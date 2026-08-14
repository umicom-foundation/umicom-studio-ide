/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_distribution_views_commands.c
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
#include "umicom/studio/distribution_commands.h"
#include "umicom/studio/distribution_views.h"

int main(void)
{
    UmiStudioDistributionViews views;
    const UmiStudioDistributionView *view;
    umi_studio_distribution_views_init(&views);
    assert(views.count == 8U);
    view = umi_studio_distribution_views_find(&views, "installers");
    assert(view != NULL);
    assert(strcmp(view->title, "Installer Centre") == 0);
    assert(!umi_studio_distribution_command_enabled(
        UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE, 0, 0));
    assert(umi_studio_distribution_command_enabled(
        UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE, 1, 0));
    assert(umi_studio_distribution_command_enabled(
        UMI_STUDIO_DISTRIBUTION_ROLLBACK, 0, 1));
    return 0;
}

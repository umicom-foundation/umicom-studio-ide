/*-----------------------------------------------------------------------------
 * Umicom Studio thin Framework navigation view contribution tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/navigation_view_contribution.h"

int main(void)
{
    size_t index;
    assert(umi_studio_navigation_view_contribution_count() == 8U);
    for (index = 0U;
         index < umi_studio_navigation_view_contribution_count();
         ++index) {
        const UmiStudioNavigationViewContribution *contribution =
            umi_studio_navigation_view_contribution_at(index);
        size_t comparison;
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_API_VERSION);
        assert(contribution->view_id[0] != '\0');
        assert(contribution->framework_contract[0] != '\0');
        assert(strcmp(contribution->default_region, "editor-overlay") == 0 ||
               strcmp(contribution->default_region,
                      "secondary-sidebar") == 0);
        assert(contribution->closable && contribution->movable);
        for (comparison = index + 1U;
             comparison < umi_studio_navigation_view_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->view_id,
                       umi_studio_navigation_view_contribution_at(
                           comparison)->view_id) != 0);
        }
    }
    assert(umi_studio_navigation_view_contribution_find(
               "studio.navigation.references") != NULL);
    assert(umi_studio_navigation_view_contribution_find(
               "studio.navigation.document-outline") != NULL);
    assert(umi_studio_navigation_view_contribution_find(
               "studio.navigation.providers") != NULL);
    assert(umi_studio_navigation_view_contribution_find(
               "studio.navigation.missing") == NULL);
    return 0;
}

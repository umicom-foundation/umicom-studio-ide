/*-----------------------------------------------------------------------------
 * Umicom Studio thin Framework source navigation contribution tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/source_navigation_contribution.h"

int main(void)
{
    size_t index;

    assert(umi_studio_source_navigation_contribution_count() ==
           umi_editor_navigation_command_count());
    for (index = 0U;
         index < umi_studio_source_navigation_contribution_count();
         ++index) {
        const UmiStudioSourceNavigationContribution *contribution =
            umi_studio_source_navigation_contribution_at(index);
        const UmiEditorNavigationCommandDescriptor *descriptor;
        size_t comparison;

        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_API_VERSION);
        descriptor = umi_editor_navigation_command_find(
            contribution->framework_command_id);
        assert(descriptor != NULL);
        assert(strcmp(contribution->menu_id, "Navigate") == 0 ||
               strcmp(contribution->menu_id, "View") == 0);
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison < umi_studio_source_navigation_contribution_count();
             ++comparison) {
            assert(strcmp(contribution->framework_command_id,
                          umi_studio_source_navigation_contribution_at(
                              comparison)->framework_command_id) != 0);
        }
    }
    assert(umi_studio_source_navigation_contribution_find(
               "editor.navigate.references") != NULL);
    assert(umi_studio_source_navigation_contribution_find(
               "editor.navigate.missing") == NULL);
    return 0;
}

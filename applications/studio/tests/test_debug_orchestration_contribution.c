/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_debug_orchestration_contribution.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/debug_orchestration_contribution.h"

int main(void)
{
    size_t index;
    size_t comparison;
    assert(umi_studio_debug_command_contribution_count() ==
           umi_debug_command_count());
    for (index = 0U; index < umi_studio_debug_command_contribution_count();
         ++index) {
        const UmiStudioDebugCommandContribution *contribution =
            umi_studio_debug_command_contribution_at(index);
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_DEBUG_ORCHESTRATION_CONTRIBUTION_API_VERSION);
        assert(umi_debug_command_find(contribution->framework_command_id) !=
               NULL);
        assert(contribution->menu_id[0] != '\0');
        assert(contribution->menu_group[0] != '\0');
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison < umi_studio_debug_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_debug_command_contribution_at(comparison)
                           ->framework_command_id) != 0);
        }
    }
    assert(umi_studio_debug_view_contribution_count() == 4U);
    for (index = 0U; index < umi_studio_debug_view_contribution_count();
         ++index) {
        const UmiStudioDebugViewContribution *view =
            umi_studio_debug_view_contribution_at(index);
        assert(view != NULL);
        assert(view->view_id[0] != '\0');
        assert(view->title[0] != '\0');
        assert(view->framework_contract[0] != '\0');
        assert(view->default_region[0] != '\0');
        assert(view->closable);
        assert(view->movable);
    }
    assert(umi_studio_debug_view_contribution_find(
               "studio.debug.session-orchestration") != NULL);
    assert(umi_studio_debug_command_contribution_find("debug.start") != NULL);
    assert(umi_studio_debug_command_contribution_at(
               umi_studio_debug_command_contribution_count()) == NULL);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_intelligence_contribution.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/editor_intelligence_contribution.h"

int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_studio_editor_intelligence_command_contribution_count() ==
           umi_editor_intelligence_command_count());
    for (index = 0U;
         index < umi_studio_editor_intelligence_command_contribution_count();
         ++index) {
        const UmiStudioEditorIntelligenceCommandContribution *contribution =
            umi_studio_editor_intelligence_command_contribution_at(index);
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION);
        assert(umi_editor_intelligence_command_find(
                   contribution->framework_command_id) != NULL);
        assert(contribution->menu_id[0] != '\0');
        assert(contribution->menu_group[0] != '\0');
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison <
                 umi_studio_editor_intelligence_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_editor_intelligence_command_contribution_at(
                           comparison)->framework_command_id) != 0);
        }
    }
    assert(umi_studio_editor_intelligence_view_contribution_count() == 2U);
    for (index = 0U;
         index < umi_studio_editor_intelligence_view_contribution_count();
         ++index) {
        const UmiStudioEditorIntelligenceViewContribution *view =
            umi_studio_editor_intelligence_view_contribution_at(index);
        assert(view != NULL);
        assert(view->view_id[0] != '\0');
        assert(view->title[0] != '\0');
        assert(view->framework_contract[0] != '\0');
        assert(view->default_region[0] != '\0');
        assert(view->closable);
        assert(view->movable);
    }
    assert(umi_studio_editor_intelligence_view_contribution_find(
               "studio.editor.rename-preview") != NULL);
    assert(umi_studio_editor_intelligence_command_contribution_find(
               "editor.intelligence.rename") != NULL);
    return 0;
}

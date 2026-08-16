/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_productivity_contribution.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/editor_productivity_contribution.h"

int main(void)
{
    size_t index;
    size_t comparison;

    assert(umi_studio_editor_productivity_command_contribution_count() ==
           umi_editor_productivity_command_count());
    for (index = 0U;
         index < umi_studio_editor_productivity_command_contribution_count();
         ++index) {
        const UmiStudioEditorProductivityCommandContribution *contribution =
            umi_studio_editor_productivity_command_contribution_at(index);
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_EDITOR_PRODUCTIVITY_CONTRIBUTION_API_VERSION);
        assert(umi_editor_productivity_command_find(
                   contribution->framework_command_id) != NULL);
        assert(contribution->menu_id[0] != '\0');
        assert(contribution->menu_group[0] != '\0');
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison <
                 umi_studio_editor_productivity_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_editor_productivity_command_contribution_at(
                           comparison)->framework_command_id) != 0);
        }
    }
    assert(umi_studio_editor_productivity_view_contribution_count() == 2U);
    for (index = 0U;
         index < umi_studio_editor_productivity_view_contribution_count();
         ++index) {
        const UmiStudioEditorProductivityViewContribution *view =
            umi_studio_editor_productivity_view_contribution_at(index);
        assert(view != NULL);
        assert(view->view_id[0] != '\0');
        assert(view->title[0] != '\0');
        assert(view->framework_contract[0] != '\0');
        assert(view->default_region[0] != '\0');
        assert(view->closable);
        assert(view->movable);
    }
    assert(umi_studio_editor_productivity_view_contribution_find(
               "studio.editor.snippet-session") != NULL);
    assert(umi_studio_editor_productivity_command_contribution_find(
               "editor.productivity.imports.organise") != NULL);
    assert(umi_studio_editor_productivity_command_contribution_at(
               umi_studio_editor_productivity_command_contribution_count()) ==
           NULL);
    return 0;
}

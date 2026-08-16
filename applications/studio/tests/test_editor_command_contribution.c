/*-----------------------------------------------------------------------------
 * Umicom Studio thin Framework editor command contribution tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/editor_command_contribution.h"

int main(void)
{
    size_t index;

    assert(umi_studio_editor_command_contribution_count() ==
           umi_editor_edit_command_count());
    for (index = 0U;
         index < umi_studio_editor_command_contribution_count();
         ++index) {
        const UmiStudioEditorCommandContribution *contribution =
            umi_studio_editor_command_contribution_at(index);
        const UmiEditorEditCommandDescriptor *descriptor;
        size_t comparison;

        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_EDITOR_COMMAND_CONTRIBUTION_API_VERSION);
        descriptor = umi_editor_edit_command_find(
            contribution->framework_command_id);
        assert(descriptor != NULL);
        assert(strcmp(contribution->menu_id, "Edit") == 0 ||
               strcmp(contribution->menu_id, "Selection") == 0);
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison < umi_studio_editor_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_editor_command_contribution_at(comparison)
                           ->framework_command_id) != 0);
        }
    }
    assert(umi_studio_editor_command_contribution_find(
               "editor.edit.toggle-line-comment") != NULL);
    assert(umi_studio_editor_command_contribution_find("editor.edit.missing") ==
           NULL);
    return 0;
}

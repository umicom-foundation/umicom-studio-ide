/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_intelligence_contribution.c
 *
 * PURPOSE:
 *   Verify that Studio contributes every Framework editor-intelligence,
 *   workspace-search, completion, inline-suggestion and diagnostics command
 *   exactly once.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/editor_intelligence_contribution.h"

static int framework_command_exists(
    const UmiStudioEditorIntelligenceCommandContribution *contribution)
{
    switch (contribution->domain) {
        case UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE:
            return umi_editor_intelligence_command_find(
                       contribution->framework_command_id) != NULL;
        case UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH:
            return umi_editor_workspace_search_command_find(
                       contribution->framework_command_id) != NULL;
        case UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION:
            return umi_editor_completion_command_find(
                       contribution->framework_command_id) != NULL;
        case UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS:
            return umi_diagnostic_command_find(
                       contribution->framework_command_id) != NULL;
        default:
            return 0;
    }
}

int main(void)
{
    const size_t expected_commands =
        umi_editor_intelligence_command_count() +
        umi_editor_workspace_search_command_count() +
        umi_editor_completion_command_count() +
        umi_diagnostic_command_count();
    size_t index;
    size_t comparison;
    size_t completion_commands = 0U;
    size_t diagnostic_commands = 0U;

    assert(umi_studio_editor_intelligence_command_contribution_count() ==
           expected_commands);
    for (index = 0U;
         index < umi_studio_editor_intelligence_command_contribution_count();
         ++index) {
        const UmiStudioEditorIntelligenceCommandContribution *contribution =
            umi_studio_editor_intelligence_command_contribution_at(index);
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION);
        assert(framework_command_exists(contribution));
        assert(contribution->menu_id[0] != '\0');
        assert(contribution->menu_group[0] != '\0');
        assert(contribution->show_in_command_centre);
        if (contribution->domain == UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION) {
            ++completion_commands;
        } else if (contribution->domain ==
                   UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS) {
            ++diagnostic_commands;
        }
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
    assert(completion_commands == umi_editor_completion_command_count());
    assert(diagnostic_commands == umi_diagnostic_command_count());
    assert(umi_studio_editor_intelligence_view_contribution_count() == 16U);
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
        assert(view->domain >= UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE);
        assert(view->domain <= UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS);
    }
    assert(umi_studio_editor_intelligence_view_contribution_find(
               "studio.editor.completion-providers") != NULL);
    assert(umi_studio_editor_intelligence_view_contribution_find(
               "studio.editor.inline-suggestion-status") != NULL);
    assert(umi_studio_editor_intelligence_command_contribution_find(
               "editor.completion.trigger") != NULL);
    assert(umi_studio_editor_intelligence_command_contribution_find(
               "replace.workspace.apply") != NULL);
    assert(umi_studio_editor_intelligence_command_contribution_find(
               "diagnostics.sarif.export") != NULL);
    assert(umi_studio_editor_intelligence_view_contribution_find(
               "studio.diagnostics.problems") != NULL);
    assert(umi_studio_editor_intelligence_view_contribution_find(
               "studio.diagnostics.severity-policy") != NULL);
    return 0;
}

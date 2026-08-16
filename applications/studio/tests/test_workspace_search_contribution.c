/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workspace_search_contribution.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/workspace_search_contribution.h"

int main(void)
{
    size_t position;
    assert(umi_studio_workspace_search_command_contribution_count() ==
           umi_editor_workspace_search_command_count());
    for (position = 0U;
         position < umi_studio_workspace_search_command_contribution_count();
         ++position) {
        const UmiStudioWorkspaceSearchCommandContribution *contribution =
            umi_studio_workspace_search_command_contribution_at(position);
        assert(contribution != NULL);
        assert(contribution->struct_size == sizeof(*contribution));
        assert(contribution->api_version ==
               UMI_STUDIO_WORKSPACE_SEARCH_CONTRIBUTION_API_VERSION);
        assert(umi_editor_workspace_search_command_find(
                   contribution->framework_command_id) != NULL);
        assert(umi_studio_workspace_search_command_contribution_find(
                   contribution->framework_command_id) == contribution);
    }
    assert(umi_studio_workspace_search_view_contribution_count() == 4U);
    assert(umi_studio_workspace_search_view_contribution_find(
               "studio.replace.preview") != NULL);
    assert(strcmp(umi_studio_workspace_search_view_contribution_find(
                      "studio.replace.transaction")->default_region,
                  "bottom-panel") == 0);
    return 0;
}

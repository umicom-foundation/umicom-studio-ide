/* Umicom Studio IDE Tests | Workspace catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = umi_test_seeded_professional_workspace_v2();
    UmiUiWorkspaceCustomisationV2 *model = umi_studio_professional_workspace_v2_model(workspace);
    const UmiUiWindowDescriptor *matches[8U];
    assert(model->windows.count == 18U);
    assert(umi_ui_window_catalogue_v2_search(&model->windows,"compare",0,matches,8U) == 1U);
    assert(strcmp(matches[0]->tool_id,"file-compare") == 0);
    assert(umi_ui_window_catalogue_v2_find(&model->windows,"order-entry") != NULL);
    umi_studio_professional_workspace_v2_destroy(workspace);
    return 0;
}

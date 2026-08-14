/* Umicom Studio IDE Tests | Workspace linked groups v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = umi_test_seeded_professional_workspace_v2();
    UmiUiWorkspaceCustomisationV2 *model = umi_studio_professional_workspace_v2_model(workspace);
    const char *routes[8U];
    size_t routed = umi_ui_window_group_v2_route(&model->groups,"project-blue","project-explorer",routes,8U);
    assert(model->groups.count == 3U);
    assert(routed == 2U);
    assert(strcmp(routes[0],"editor-main") == 0);
    umi_studio_professional_workspace_v2_destroy(workspace);
    return 0;
}

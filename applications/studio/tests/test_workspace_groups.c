/* Umicom Studio IDE Tests | Workspace linked groups v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspace *workspace = umi_test_seeded_professional_workspace();
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    const char *routes[8U];
    size_t routed = umi_ui_window_group_route(&model->groups,"project-blue","project-explorer",routes,8U);
    assert(model->groups.count == 15U);
    assert(routed == 2U);
    assert(strcmp(routes[0],"editor-main") == 0);
    routed = umi_ui_window_group_route(&model->groups,"trading-teal","trading-watchlists",routes,8U);
    assert(routed == 4U);
    assert(strcmp(routes[0],"trading-chart") == 0);
    routed = umi_ui_window_group_route(
        &model->groups,"treasury-blue","tms-blotter",routes,8U);
    assert(routed == 4U);
    umi_studio_professional_workspace_destroy(workspace);
    return 0;
}

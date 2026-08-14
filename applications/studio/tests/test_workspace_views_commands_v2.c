/* Umicom Studio IDE Tests | Workspace views and commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
#include "umicom/studio/workspace_views_v2.h"
int main(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = umi_test_seeded_professional_workspace_v2();
    UmiStudioWorkspaceViewV2 view;
    UmiStudioProfessionalWorkspaceSnapshotV2 snapshot;
    assert(umi_studio_workspace_view_v2_count() == 6U);
    assert(umi_studio_workspace_view_v2_resolve(workspace,"new-window",&view) == UMI_STATUS_OK);
    assert(view.item_count == 18U);
    assert(umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_THEME_LIGHT) == UMI_STATUS_OK);
    assert(umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_OPERATIONS) == UMI_STATUS_OK);
    assert(umi_studio_professional_workspace_v2_snapshot(workspace,&snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.customisation.active_layout_id,"operations") == 0);
    assert(strcmp(snapshot.customisation.theme_id,"umicom-light") == 0);
    umi_studio_professional_workspace_v2_destroy(workspace);
    return 0;
}

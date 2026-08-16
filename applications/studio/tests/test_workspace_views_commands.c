/* Umicom Studio IDE Tests | Workspace views and commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
#include "umicom/studio/workspace_views.h"
int main(void)
{
    UmiStudioProfessionalWorkspace *workspace = umi_test_seeded_professional_workspace();
    UmiStudioWorkspaceView view;
    UmiStudioProfessionalWorkspaceSnapshot snapshot;
    assert(umi_studio_workspace_view_count() == 6U);
    assert(umi_studio_workspace_view_resolve(workspace,"new-window",&view) == UMI_STATUS_OK);
    assert(umi_studio_professional_workspace_snapshot(workspace,&snapshot) == UMI_STATUS_OK);
    assert(view.item_count == snapshot.customisation.available_windows);
    assert(view.item_count >= 20U);
    assert(umi_studio_workspace_execute(workspace,UMI_STUDIO_WORKSPACE_COMMAND_THEME_LIGHT) == UMI_STATUS_OK);
    assert(umi_studio_workspace_execute(workspace,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_OPERATIONS) == UMI_STATUS_OK);
    assert(umi_studio_professional_workspace_snapshot(workspace,&snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.customisation.active_layout_id,"operations") == 0);
    assert(strcmp(snapshot.customisation.theme_id,"umicom-light") == 0);
    umi_studio_professional_workspace_destroy(workspace);
    return 0;
}

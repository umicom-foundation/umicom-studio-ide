/* Umicom Studio IDE Tests | Workspace layouts v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = umi_test_seeded_professional_workspace_v2();
    UmiUiWorkspaceCustomisationV2 *model = umi_studio_professional_workspace_v2_model(workspace);
    UmiUiWorkspaceLayoutV3 *active;
    assert(model->layout_count == 3U && model->library.count == 3U);
    assert(umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE) == UMI_STATUS_OK);
    active = umi_ui_workspace_customisation_v2_active(model);
    assert(active != NULL && strcmp(active->layout_id,"compare") == 0);
    assert(active->window_count == 4U && active->locked);
    assert(umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_UNLOCK) == UMI_STATUS_OK);
    assert(!active->locked);
    assert(umi_studio_workspace_execute_v2(workspace,UMI_STUDIO_WORKSPACE_COMMAND_LOCK) == UMI_STATUS_OK);
    umi_studio_professional_workspace_v2_destroy(workspace);
    return 0;
}

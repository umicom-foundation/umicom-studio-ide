/* Umicom Studio IDE Tests | Workspace persistence v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
#include "umicom/studio/workspace_persistence_v2.h"
int main(void)
{
    UmiStudioProfessionalWorkspaceV2 *source = umi_test_seeded_professional_workspace_v2();
    UmiStudioProfessionalWorkspaceV2 *target = umi_test_professional_workspace_v2();
    UmiUiWorkspaceCustomisationV2 *target_model = umi_studio_professional_workspace_v2_model(target);
    char encoded[UMI_UI_LAYOUT_ENCODED_CAPACITY];
    assert(umi_studio_workspace_execute_v2(source,UMI_STUDIO_WORKSPACE_COMMAND_ACTIVATE_COMPARE) == UMI_STATUS_OK);
    assert(umi_studio_workspace_persistence_v2_export_active(source,5700U,encoded,sizeof(encoded)) == UMI_STATUS_OK);
    assert(strstr(encoded,"UMILAYOUT2") != NULL);
    assert(umi_studio_workspace_persistence_v2_import(target,encoded,true) == UMI_STATUS_OK);
    assert(target_model->layout_count == 1U);
    assert(strcmp(target_model->active_layout_id,"compare") == 0);
    assert(target_model->layouts[0].window_count == 4U);
    umi_studio_professional_workspace_v2_destroy(target);
    umi_studio_professional_workspace_v2_destroy(source);
    return 0;
}

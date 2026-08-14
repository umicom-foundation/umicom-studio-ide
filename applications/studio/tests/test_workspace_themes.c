/* Umicom Studio IDE Tests | Workspace themes v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspace *workspace = umi_test_seeded_professional_workspace();
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    assert(umi_studio_workspace_theme_apply(workspace,UMI_STUDIO_WORKSPACE_THEME_HIGH_CONTRAST,UMI_UI_DENSITY_COMPACT,1.25) == UMI_STATUS_OK);
    assert(model->theme.mode == UMI_UI_THEME_MODE_HIGH_CONTRAST);
    assert(model->theme.font_scale == 1.25);
    assert(strcmp(model->theme.accent,"#ffff00") == 0);
    umi_studio_professional_workspace_destroy(workspace);
    return 0;
}

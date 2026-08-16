/* Umicom Studio IDE Tests | Workspace catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
int main(void)
{
    UmiStudioProfessionalWorkspace *workspace = umi_test_seeded_professional_workspace();
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    const UmiUiWindowDescriptor *matches[8U];
    assert(model->windows.count >= 90U);
    assert(umi_ui_window_catalogue_search(
        &model->windows,"compare",0,matches,8U) >= 1U);
    assert(umi_ui_window_catalogue_find(
        &model->windows,"file-compare") != NULL);
    assert(umi_ui_window_catalogue_find(&model->windows,"order-entry") != NULL);
    assert(umi_ui_window_catalogue_find(&model->windows,"portfolio") != NULL);
    assert(umi_ui_window_catalogue_find(&model->windows,"trading-activity") != NULL);
    assert(umi_ui_window_catalogue_find(
        &model->windows,"umicom.treasury.trade-capture") != NULL);
    assert(umi_ui_window_catalogue_find(
        &model->windows,"umicom.media.timeline") != NULL);
    assert(umi_ui_window_catalogue_find(
        &model->windows,"umicom.creator.canvas") != NULL);
    assert(umi_ui_window_catalogue_find(
        &model->windows,"umicom.cad.viewport") != NULL);
    umi_studio_professional_workspace_destroy(workspace);
    return 0;
}

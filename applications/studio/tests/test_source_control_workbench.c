/*-----------------------------------------------------------------------------
 * Verify Studio consumes all Framework Source Control Centre view factories.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/ui/view_presentation.h"
static void verify_kind(UmiUiWorkbench *workbench, const char *view_type, const char *pane_id, const char *expected)
{
    UmiUiViewPresentation *presentation = calloc(1U, sizeof(*presentation)); UmiUiPropertySnapshot kind;
    assert(presentation != NULL);
    assert(umi_ui_view_presentation_build(umi_ui_workbench_view_factories(workbench), view_type, pane_id, presentation) == UMI_STATUS_OK);
    assert(umi_ui_view_presentation_find_property(presentation, "umicom.view-kind", &kind) == UMI_STATUS_OK);
    assert(kind.value.kind == UMI_UI_VALUE_STRING); assert(strcmp(kind.value.string_value, expected) == 0);
    free(presentation);
}
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL; UmiUiWorkbench *workbench;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap)); assert(workbench != NULL);
    verify_kind(workbench, "studio.source-control", UMI_STUDIO_PANE_SOURCE_CONTROL, "source-control-changes");
    verify_kind(workbench, "studio.vcs-commit", UMI_STUDIO_PANE_VCS_COMMIT, "source-control-commit");
    verify_kind(workbench, "studio.vcs-history", UMI_STUDIO_PANE_VCS_HISTORY, "source-control-history");
    verify_kind(workbench, "studio.vcs-branches", UMI_STUDIO_PANE_VCS_BRANCHES, "source-control-branches");
    verify_kind(workbench, "studio.vcs-remotes", UMI_STUDIO_PANE_VCS_REMOTES, "source-control-remotes");
    verify_kind(workbench, "studio.vcs-conflicts", UMI_STUDIO_PANE_VCS_CONFLICTS, "source-control-conflicts");
    verify_kind(workbench, "studio.vcs-diff", UMI_STUDIO_PANE_VCS_DIFF, "source-control-diff");
    verify_kind(workbench, "studio.vcs-operations", UMI_STUDIO_PANE_VCS_OPERATIONS, "source-control-operations");
    umi_studio_bootstrap_destroy(bootstrap); return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/workbench_main.c
 *
 * PURPOSE:
 *   Provide a native headless command for inspecting and exercising the Studio
 *   workbench without requiring GTK4.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui.h"

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioUi *ui;
    UmiStudioUiSnapshot snapshot;
    UmiUiHeadlessSnapshot render;
    UmiStatus status;
    int render_requested = 0;
    int index;

    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--render") == 0) render_requested = 1;
    }
    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Workbench startup failed: %s\n", umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    ui = umi_studio_bootstrap_ui(bootstrap);
    status = umi_studio_ui_refresh(ui);
    if (status == UMI_STATUS_OK) status = umi_studio_ui_snapshot(ui, &snapshot);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Workbench inspection failed: %s\n", umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    (void)printf("Studio Workbench\nPerspectives: %zu\nPanes: %zu\nActions: %zu\n"
                 "Menus: %zu\nToolbars: %zu\nStatus items: %zu\n"
                 "Contributions: %zu\nActive perspective: %s\n",
                 snapshot.perspectives, snapshot.panes, snapshot.actions,
                 snapshot.menus, snapshot.toolbars, snapshot.status_items,
                 snapshot.contributions, snapshot.active_perspective);
    if (render_requested) {
        status = umi_studio_ui_render_headless(ui, &render);
        if (status == UMI_STATUS_OK) (void)printf("\n%s", render.text);
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

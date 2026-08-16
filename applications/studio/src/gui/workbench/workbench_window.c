/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/workbench/workbench_window.c
 *
 * PURPOSE:
 *   Present the Studio Framework application shell through the GTK4 reference
 *   adapter and own its adapter lifetime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_window.h"

#include <stdlib.h>

struct UmiStudioGtkWorkbench {
    UmiStudioUi *ui;
    UmiGtk4Adapter *adapter;
    GtkWindow *window;
};

UmiStatus umi_studio_gtk_workbench_create(GtkApplication *application,
                                          UmiStudioUi *ui,
                                          UmiDesktopShellModel *desktop_shell,
                                          UmiStudioGtkWorkbench **out_workbench)
{
    UmiStudioGtkWorkbench *workbench;
    UmiStatus status;
    if (application == NULL || ui == NULL || desktop_shell == NULL ||
        out_workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_workbench = NULL;
    workbench = (UmiStudioGtkWorkbench *)calloc(1U, sizeof(*workbench));
    if (workbench == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workbench->ui = ui;
    status = umi_gtk4_adapter_create(application, &workbench->adapter);
    if (status == UMI_STATUS_OK) {
        status = umi_gtk4_adapter_bind_desktop_shell(
            workbench->adapter, desktop_shell);
    }
    if (status == UMI_STATUS_OK) status = umi_gtk4_adapter_present(workbench->adapter, umi_studio_ui_shell(ui));
    if (status != UMI_STATUS_OK) {
        umi_studio_gtk_workbench_destroy(workbench);
        return status;
    }
    workbench->window = GTK_WINDOW(umi_gtk4_adapter_native_window(workbench->adapter));
    *out_workbench = workbench;
    return UMI_STATUS_OK;
}

void umi_studio_gtk_workbench_destroy(UmiStudioGtkWorkbench *workbench)
{
    if (workbench == NULL) return;
    umi_gtk4_adapter_destroy(workbench->adapter);
    free(workbench);
}

GtkWindow *umi_studio_gtk_workbench_window(UmiStudioGtkWorkbench *workbench)
{
    return workbench != NULL ? workbench->window : NULL;
}

UmiStatus umi_studio_gtk_workbench_refresh(UmiStudioGtkWorkbench *workbench)
{
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_ui_refresh(workbench->ui);
    if (status != UMI_STATUS_OK) return status;
    return umi_gtk4_adapter_refresh(workbench->adapter);
}

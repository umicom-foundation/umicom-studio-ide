/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/workbench/include/workbench_window.h
 *
 * PURPOSE:
 *   Declare the private GTK4 host for the Framework-backed Studio workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_GTK_WORKBENCH_WINDOW_H
#define UMICOM_STUDIO_GTK_WORKBENCH_WINDOW_H

#include <gtk/gtk.h>
#include "umicom/studio/ui.h"

typedef struct UmiStudioGtkWorkbench UmiStudioGtkWorkbench;
UmiStatus umi_studio_gtk_workbench_create(GtkApplication *application,
                                          UmiStudioUi *ui,
                                          UmiDesktopShellModel *desktop_shell,
                                          UmiStudioGtkWorkbench **out_workbench);
void umi_studio_gtk_workbench_destroy(UmiStudioGtkWorkbench *workbench);
GtkWindow *umi_studio_gtk_workbench_window(UmiStudioGtkWorkbench *workbench);
UmiStatus umi_studio_gtk_workbench_refresh(UmiStudioGtkWorkbench *workbench);

#endif

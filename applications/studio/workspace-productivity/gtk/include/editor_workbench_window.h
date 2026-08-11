/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/gtk/include/editor_workbench_window.h
 *
 * PURPOSE:
 *   Expose the GTK4/GtkSourceView productivity workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#ifndef UMICOM_STUDIO_EDITOR_WORKBENCH_WINDOW_H
#define UMICOM_STUDIO_EDITOR_WORKBENCH_WINDOW_H

#include <gtk/gtk.h>

GtkWidget *umi_studio_editor_workbench_window_new(GtkApplication *application);

#endif

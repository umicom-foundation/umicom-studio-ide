/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/explorer/include/project_explorer_view.h
 *
 * PURPOSE:
 *   Declare a GTK4 projection of the Framework workspace and file-index state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_GTK_PROJECT_EXPLORER_VIEW_H
#define UMICOM_STUDIO_GTK_PROJECT_EXPLORER_VIEW_H

#include <gtk/gtk.h>
#include "umicom/studio/ui.h"

GtkWidget *umi_studio_project_explorer_view_new(UmiStudioUi *ui);

#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/include/inspector_view.h
 *
 * PURPOSE:
 *   Declare the GTK4 property inspector for the current primary semantic component.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_INSPECTOR_VIEW_H
#define UMICOM_STUDIO_DESIGNER_INSPECTOR_VIEW_H
#include <gtk/gtk.h>
#include "umicom/studio/designer.h"
GtkWidget *umi_studio_designer_inspector_view_new(UmiStudioDesigner *designer,const char *node_id);
#endif

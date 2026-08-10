/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/include/designer_window.h
 *
 * PURPOSE:
 *   Declare the GTK4 reference visual-designer window composed entirely from Framework-backed Studio services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_DESIGNER_WINDOW_H
#define UMICOM_STUDIO_DESIGNER_DESIGNER_WINDOW_H
#include <gtk/gtk.h>
#include "umicom/studio/designer.h"
GtkWidget *umi_studio_designer_window_new(GtkApplication *application,UmiStudioDeclarative *declarative,UmiStudioDesigner *designer);
#endif

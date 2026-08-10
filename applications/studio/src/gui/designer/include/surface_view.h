/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/include/surface_view.h
 *
 * PURPOSE:
 *   Declare a GTK4 design-surface view that visualises semantic component rectangles without owning application state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_SURFACE_VIEW_H
#define UMICOM_STUDIO_DESIGNER_SURFACE_VIEW_H
#include <gtk/gtk.h>
#include "umicom/studio/designer.h"
GtkWidget *umi_studio_designer_surface_view_new(UmiStudioDesigner *designer);
#endif

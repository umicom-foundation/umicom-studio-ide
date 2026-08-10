/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/include/palette_view.h
 *
 * PURPOSE:
 *   Declare the GTK4 palette view that renders semantic component metadata from Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_PALETTE_VIEW_H
#define UMICOM_STUDIO_DESIGNER_PALETTE_VIEW_H
#include <gtk/gtk.h>
#include "umicom/studio/designer.h"
GtkWidget *umi_studio_designer_palette_view_new(UmiStudioDeclarative *declarative);
#endif

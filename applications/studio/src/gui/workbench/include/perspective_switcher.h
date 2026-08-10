/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/workbench/include/perspective_switcher.h
 *
 * PURPOSE:
 *   Declare a GTK4 perspective selector backed by the Framework perspective model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_GTK_PERSPECTIVE_SWITCHER_H
#define UMICOM_STUDIO_GTK_PERSPECTIVE_SWITCHER_H

#include <gtk/gtk.h>
#include "umicom/studio/ui.h"

GtkWidget *umi_studio_perspective_switcher_new(UmiStudioUi *ui);

#endif

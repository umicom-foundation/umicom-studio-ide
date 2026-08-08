/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/dialog/include/about.h
 *
 * PURPOSE:
 *   Public helper to present the GTK4 About dialog in a non-blocking way.
 *
 * API:
 *   void umi_about_show(GtkWindow *parent);
 *   void umi_show_about(GtkWindow *parent); // legacy alias
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ABOUT_H
#define UMICOM_ABOUT_H

#include <gtk/gtk.h>

void umi_about_show (GtkWindow *parent);
void umi_show_about (GtkWindow *parent); /* legacy alias retained */

#endif /* UMICOM_ABOUT_H */

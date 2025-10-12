/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/window.c
 * PURPOSE: Minimal window helper (pure C widgets; no GtkBuilder/resources)
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/

#include <gtk/gtk.h>

GtkWidget* window_new(GtkApplication *app) {
  GtkWidget *win = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(win), "Umicom Studio IDE");
  gtk_window_set_default_size(GTK_WINDOW(win), 1200, 800);
  return win;
}

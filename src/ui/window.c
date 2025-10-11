/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/

#include "../include/window.h"

static GtkWidget* build_ui(GtkApplication *app) {
  GtkBuilder *builder = gtk_builder_new_from_resource("/com/umicom/ustudio/ui/main.ui");
  GtkWidget *win = GTK_WIDGET(gtk_builder_get_object(builder, "app_window"));
  gtk_window_set_application(GTK_WINDOW(win), app);
  g_object_unref(builder);
  return win;
}

GtkWidget* window_new(GtkApplication *app) {
  GtkWidget *win = build_ui(app);
  return win;
}

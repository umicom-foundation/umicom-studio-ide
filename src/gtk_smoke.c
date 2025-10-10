\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gtk_smoke.c
 * PURPOSE: Minimal GTK 'smoke test' app (dev only)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>

static void on_activate(GtkApplication *app, gpointer user_data) {
  (void)user_data;
  GtkWidget *win = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(win), "USIDE smoke");
  gtk_window_set_default_size(GTK_WINDOW(win), 400, 200);
  gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char **argv) {
  (void)argc; (void)argv;
  GtkApplication *app = gtk_application_new("com.umicom.ustudio", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), 0, NULL);
  g_object_unref(app);
  return status;
}


#include <gtk/gtk.h>
#include "window.h"

static void on_activate (GApplication *app, gpointer user_data) {
  GtkWindow *win = GTK_WINDOW(window_new(GTK_APPLICATION(app)));
  gtk_window_present(win);
}

int app_run(int argc, char **argv) {
  g_autoptr(GtkApplication) app = gtk_application_new("com.umicom.ustudio", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}

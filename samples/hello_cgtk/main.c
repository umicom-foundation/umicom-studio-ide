/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: samples/hello_cgtk/main.c
 * PURPOSE: Minimal GTK app to validate toolchain
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
int main(int argc, char **argv){
  gtk_init();
  GtkWidget *w = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(w), "Hello Umicom");
  g_signal_connect(w, "close-request", G_CALLBACK(gtk_window_destroy), NULL);
  gtk_widget_show(w);
  while(gtk_events_pending()) gtk_main_iteration();
  return 0;
}

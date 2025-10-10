/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: gtk_smoke.c
 * PURPOSE: Minimal GTK smoke test (no deprecations).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>

static void on_activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;
    GtkWidget *win = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(win), "USIDE Smoke Test");
    gtk_window_set_default_size(GTK_WINDOW(win), 640, 400);
    gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char **argv){
    GtkApplication *app = gtk_application_new("com.umicom.ustudio", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

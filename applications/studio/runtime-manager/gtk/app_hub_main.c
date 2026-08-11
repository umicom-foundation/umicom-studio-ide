/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/gtk/app_hub_main.c
 *
 * PURPOSE:
 *   Start the standalone GTK4 Studio Application Hub frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include <gtk/gtk.h>

#include "app_hub_window.h"

static void on_activate(GtkApplication *application, gpointer user_data)
{
    GtkWidget *window;

    (void)user_data;
    window = umi_studio_app_hub_window_new(application);
    if (window != NULL) {
        gtk_window_present(GTK_WINDOW(window));
    }
}

int main(int argc, char **argv)
{
    GtkApplication *application;
    int result;

    application = gtk_application_new(
        "org.umicom.studio.application-hub",
        G_APPLICATION_DEFAULT_FLAGS
    );
    if (application == NULL) {
        return 1;
    }

    g_signal_connect(application, "activate", G_CALLBACK(on_activate), NULL);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}

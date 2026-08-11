/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/gtk/main.c
 *
 * PURPOSE:
 *   Start the Studio workspace/editor productivity GTK4 demo.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include <gtk/gtk.h>

#if defined(UMICOM_STUDIO_HAS_SOURCEVIEW)
#include <gtksourceview/gtksource.h>
#endif

#include "editor_workbench_window.h"

static void on_activate(GtkApplication *application, gpointer user_data)
{
    GtkWidget *window;

    (void)user_data;
    window = umi_studio_editor_workbench_window_new(application);
    if (window != NULL) {
        gtk_window_present(GTK_WINDOW(window));
    }
}

int main(int argc, char **argv)
{
    GtkApplication *application;
    int result;

#if defined(UMICOM_STUDIO_HAS_SOURCEVIEW)
    gtk_source_init();
#endif

    application = gtk_application_new(
        "org.umicom.studio.workspace-productivity",
        G_APPLICATION_DEFAULT_FLAGS
    );
    if (application == NULL) {
        return 1;
    }

    g_signal_connect(application, "activate", G_CALLBACK(on_activate), NULL);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);

#if defined(UMICOM_STUDIO_HAS_SOURCEVIEW)
    gtk_source_finalize();
#endif

    return result;
}

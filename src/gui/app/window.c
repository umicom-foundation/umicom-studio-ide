/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/window.c
 *
 * PURPOSE:
 *   Minimal main-window helper built with GTK4 primitives (no GtkBuilder).
 *   Creates a sane default toplevel used by the app shell. Pure C, no CSS.
 *
 * DESIGN:
 *   - Tiny surface: a single factory function.
 *   - Defensive guards for NULL app pointer (returns NULL).
 *
 * API:
 *   GtkWidget *window_new(GtkApplication *app);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>   /* GtkApplicationWindow, gtk_window_* APIs */

/* Create and configure the main application window.
 * Returns: a GtkApplicationWindow (as GtkWidget*) or NULL if 'app' is NULL. */
GtkWidget *window_new(GtkApplication *app)
{
    if (!app) return NULL;                        /* guard against misuse           */

    /* Create an application-scoped toplevel (GTK4 API).                      */
    GtkWidget *win = gtk_application_window_new(app);

    /* Title + default size (tweak as needed).                                */
    gtk_window_set_title(GTK_WINDOW(win), "Umicom Studio IDE");
    gtk_window_set_default_size(GTK_WINDOW(win), 1200, 800);

    /* Callers remain owners of the reference returned; GTK manages children. */
    return win;
}
/*-----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
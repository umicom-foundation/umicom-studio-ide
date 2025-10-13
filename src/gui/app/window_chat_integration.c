/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/window_chat_integration.c
 *
 * PURPOSE:
 *   Wire the Chat pane into the main window and register an application
 *   action "app.toggle-chat" that shows/hides the right-side box.
 *   Pure C; no CSS or style classes.
 *
 * DESIGN:
 *   - GTK4 only; no deprecated APIs.
 *   - Avoid brittle casts: action callbacks use exact expected signatures.
 *   - The right_box is optional; when provided, we toggle its visibility.
 *
 * SECURITY/ROBUSTNESS:
 *   - All pointers are guarded.
 *   - No unsafe casts; no vararg format misuse.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <gio/gio.h>
#include "window_chat_integration.h"  /* our API */

/* Toggle action: show/hide the right pane when invoked. */
static void toggle_chat(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action; (void)parameter;
    GtkWidget *right_box = GTK_WIDGET(user_data);
    if (!right_box) return;

    gboolean visible = gtk_widget_get_visible(right_box);
    gtk_widget_set_visible(right_box, !visible);
}

/* Single, stateless action entry for the application scope. */
static const GActionEntry k_entries[] = {
    { .name = "toggle-chat", .activate = toggle_chat }
};

/* Public entry point: mount the action and (optionally) a pane widget. */
void ustudio_chat_mount_and_actions(GtkApplication *app, GtkWindow *win, GtkWidget *right_box)
{
    (void)win; /* reserved for future behavior (e.g., transient dialogs) */

    if (!app) return;

    /* Register the action at the application scope; user_data carries the pane. */
    g_action_map_add_action_entries(G_ACTION_MAP(app),
                                    k_entries,
                                    (gint)G_N_ELEMENTS(k_entries),
                                    right_box /* user_data for activate */);

    /* If a right_box is provided but currently invisible, make it visible. */
    if (right_box && !gtk_widget_get_visible(right_box)) {
        gtk_widget_set_visible(right_box, TRUE);
    }
}
/*-----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
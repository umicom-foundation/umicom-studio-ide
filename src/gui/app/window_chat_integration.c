
/* ---------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/window_chat_integration.c
 * PURPOSE: Wire chat pane & actions into the main window (GTK4)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *
 * CHANGELOG (compile-only fix):
 * - Remove invalid (GCallback) cast on GActionEntry.activate.
 * - Use designated initializer and include trailing padding to silence the
 *   'missing field padding initializer' warning with GLib/GTK versions that
 *   expose it.
 * - Keep all logic minimal and non-invasive. Add comments for clarity.
 * --------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <gio/gio.h>

/* Exact signature expected by GActionEntry.activate */
static void toggle_chat(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter; (void)user_data;
    /* TODO: implement actual UI toggle for chat pane if needed. */
}

/* Designated initializer + explicit padding zero-init to avoid warnings */
static const GActionEntry entries[] = {
    {
        .name = "toggle-chat",
        .activate = toggle_chat,
        .parameter_type = NULL,
        .state = "true",
        .change_state = NULL,
        .padding = {0}
    }
};

void ustudio_chat_mount_and_actions(GtkApplication *app, GtkWindow *win, GtkWidget *right_box) {
    (void)win; (void)right_box; /* not used here; reserved for real wiring */
    g_action_map_add_action_entries(G_ACTION_MAP(app), entries, G_N_ELEMENTS(entries), NULL);
}

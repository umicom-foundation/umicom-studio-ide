#include "window_chat_integration.h"
#include "panes/chat_pane.h"
#include <gio/gio.h>

typedef struct {
    GtkWidget *chat;
    gboolean   visible;
    GtkWidget *right_box;
} ChatMount;

static void toggle_chat(GSimpleAction *action, GVariant *state, gpointer user_data) {
    ChatMount *cm = (ChatMount*)user_data;
    cm->visible = !cm->visible;
    g_simple_action_set_state(action, g_variant_new_boolean(cm->visible));
    if (!cm->chat) return;
    gtk_widget_set_visible(cm->chat, cm->visible);
}

void ustudio_chat_mount_and_actions(GtkApplication *app, GtkWindow *win, GtkWidget *right_box) {
    if (!app || !right_box) return;

    ChatMount *cm = g_new0(ChatMount, 1);
    cm->right_box = right_box;
    cm->chat = chat_pane_new();
    cm->visible = TRUE;
    gtk_widget_set_hexpand(cm->chat, TRUE);
    gtk_widget_set_vexpand(cm->chat, TRUE);
    gtk_box_append(GTK_BOX(right_box), cm->chat);

    // Action: app.toggle-chat (stateful)
    const GActionEntry entries[] = {
        { "toggle-chat", NULL, NULL, "true", toggle_chat },
    };
    g_action_map_add_action_entries(G_ACTION_MAP(app), entries, G_N_ELEMENTS(entries), cm);

    // Accelerator: <Ctrl><Shift>C
    const char *accels[] = { "<Ctrl><Shift>C", NULL };
    gtk_application_set_accels_for_action(app, "app.toggle-chat", accels);
}

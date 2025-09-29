
#ifndef USTUDIO_WINDOW_CHAT_INTEGRATION_H
#define USTUDIO_WINDOW_CHAT_INTEGRATION_H

#include <gtk/gtk.h>

/* Mount chat pane into right_box and add a toggle action (app.toggle-chat) on the app. */
void ustudio_chat_mount_and_actions(GtkApplication *app, GtkWindow *win, GtkWidget *right_box);

#endif

/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/

#ifndef USTUDIO_WINDOW_CHAT_INTEGRATION_H
#define USTUDIO_WINDOW_CHAT_INTEGRATION_H

#include <gtk/gtk.h>

/* Mount chat pane into right_box and add a toggle action (app.toggle-chat) on the app. */
void ustudio_chat_mount_and_actions(GtkApplication *app, GtkWindow *win, GtkWidget *right_box);

#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/include/window_chat_integration.h
 *
 * PURPOSE:
 *   Public declaration to mount the Chat pane (right side) and to register
 *   a 'toggle-chat' application action. Pure C; no CSS.
 *
 * API:
 *   void ustudio_chat_mount_and_actions(GtkApplication *app,
 *                                       GtkWindow      *win,
 *                                       GtkWidget      *right_box);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 *---------------------------------------------------------------------------*/
#ifndef USTUDIO_WINDOW_CHAT_INTEGRATION_H
#define USTUDIO_WINDOW_CHAT_INTEGRATION_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void ustudio_chat_mount_and_actions(GtkApplication *app,
                                    GtkWindow      *win,
                                    GtkWidget      *right_box);

G_END_DECLS
#endif /* USTUDIO_WINDOW_CHAT_INTEGRATION_H */

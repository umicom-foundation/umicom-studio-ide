/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/window_chat_integration.h
 *
 * PURPOSE:
 *   Public declaration to mount the Chat pane (right side) and to register
 *   a 'toggle-chat' application action.
 *
 * API:
 *   void ustudio_chat_mount_and_actions(GtkApplication *app,
 *                                       GtkWindow      *win,
 *                                       GtkWidget      *right_box);
 *
 * NOTES:
 *   This header is intentionally thin; implementation lives in the .c file.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 *---------------------------------------------------------------------------*/
#ifndef USTUDIO_WINDOW_CHAT_INTEGRATION_H
#define USTUDIO_WINDOW_CHAT_INTEGRATION_H

#include <gtk/gtk.h>

void ustudio_chat_mount_and_actions(GtkApplication *app,
                                    GtkWindow      *win,
                                    GtkWidget      *right_box);

#endif /* USTUDIO_WINDOW_CHAT_INTEGRATION_H */

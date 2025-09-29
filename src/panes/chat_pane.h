
#ifndef USTUDIO_CHAT_PANE_H
#define USTUDIO_CHAT_PANE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Create a new Chat Pane widget.
 * The pane expects a simple HTTP chat sidecar at http://127.0.0.1:8750/chat
 * accepting JSON: {"prompt":"..."} and returning {"ok":true,"reply":"..."}
 */
GtkWidget* chat_pane_new(void);

/* Optional: set endpoint (default "http://127.0.0.1:8750/chat") */
void chat_pane_set_endpoint(GtkWidget* pane, const char* endpoint_url);

G_END_DECLS

#endif /* USTUDIO_CHAT_PANE_H */

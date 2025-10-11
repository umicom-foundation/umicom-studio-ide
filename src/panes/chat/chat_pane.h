#ifndef USTUDIO_CHAT_PANE_H
#define USTUDIO_CHAT_PANE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward-declare GtkWidget so this header doesn't need gtk/gtk.h */
typedef struct _GtkWidget GtkWidget;

/* Create a new Chat Pane widget. */
GtkWidget* chat_pane_new(void);

/* Optional: set endpoint (default "http://127.0.0.1:8750/chat") */
void chat_pane_set_endpoint(GtkWidget* pane, const char* endpoint_url);

#ifdef __cplusplus
}
#endif
#endif /* USTUDIO_CHAT_PANE_H */

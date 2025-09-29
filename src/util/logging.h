
#ifndef USTUDIO_LOGGING_H
#define USTUDIO_LOGGING_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Bind GLib logs (g_message, g_warning, g_error (fatal), g_debug) to a GtkTextView. */
void ustudio_log_bind(GtkTextView *view);

G_END_DECLS
#endif

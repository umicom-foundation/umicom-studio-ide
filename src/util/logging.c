
#include "logging.h"
#include <glib.h>
#include <stdarg.h>

typedef struct {
    GtkTextView *view;
    GMutex       lock;
} LogState;

static LogState *g_state = NULL;

static void append_line(const char *line) {
    if (!g_state || !g_state->view) return;
    GtkTextBuffer *buf = gtk_text_view_get_buffer(g_state->view);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_insert(buf, &end, line, -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);
}

static void log_handler(const gchar *log_domain,
                        GLogLevelFlags log_level,
                        const gchar *message,
                        gpointer user_data) {
    (void)user_data;
    const char *lvl = "INFO";
    if (log_level & G_LOG_LEVEL_ERROR)   lvl = "ERROR";
    else if (log_level & G_LOG_LEVEL_CRITICAL) lvl = "CRIT";
    else if (log_level & G_LOG_LEVEL_WARNING)  lvl = "WARN";
    else if (log_level & G_LOG_LEVEL_DEBUG)    lvl = "DEBUG";

    char line[2048];
    if (log_domain) g_snprintf(line, sizeof line, "[%s] %s: %s", lvl, log_domain, message);
    else            g_snprintf(line, sizeof line, "[%s] %s",      lvl, message);

    g_idle_add_full(G_PRIORITY_DEFAULT, (GSourceFunc)(+[] (gpointer data) -> gboolean {
        append_line((const char*)data);
        g_free(data);
        return G_SOURCE_REMOVE;
    }), g_strdup(line), NULL);
}

void ustudio_log_bind(GtkTextView *view) {
    if (!g_state) {
        g_state = g_new0(LogState, 1);
        g_mutex_init(&g_state->lock);
    }
    g_state->view = view;

    /* Route default domain + app domain to our handler */
    g_log_set_handler(NULL, (GLogLevelFlags)(G_LOG_LEVEL_MASK), log_handler, NULL);
    g_log_set_handler("ustudio", (GLogLevelFlags)(G_LOG_LEVEL_MASK), log_handler, NULL);

    g_message("Log pane attached.");
}

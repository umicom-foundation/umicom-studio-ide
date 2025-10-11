/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/logging.c
 * PURPOSE: UI-friendly logging helpers (pure C, GTK4) with idle dispatch
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *
 * IMPORTANT
 * =========
 * - This file replaces any previous C++ lambda usage with a **pure C** idle
 *   callback so it builds with -std=gnu17. No C++ toolchain is required.
 * - All functions are small, documented, and safe to call from background
 *   threads: we hop back to the GTK main loop using g_idle_add().
 *
 * WHAT YOU CAN CUSTOMIZE
 * ----------------------
 * - If you already have a header (e.g. util/logging.h) declaring specific
 *   symbols, you can rename the exported functions below to match it.
 * - By default we provide a tiny API:
 *     void ustudio_logging_init(GtkTextView *optional_view);
 *     void ustudio_log_line(const char *line);
 *     void ustudio_log_fmt(const char *fmt, ...);
 *
 * NOTES
 * -----
 * - We gracefully handle the case where no text view is bound: messages still
 *   go to GLib logs (g_message). When a view is bound, we mirror to both.
 * - All credits/comments preserved; heavy inline comments added for clarity.
 *---------------------------------------------------------------------------*/

#include <stdarg.h>
#include <glib.h>
#include <gtk/gtk.h>

/*-----------------------------------------------------------------------------
 * Internal shared state for the UI sink.
 * We keep a single optional GtkTextView target; callers can re-bind by calling
 * ustudio_logging_init() again later with another view (e.g. when UI rebuilds).
 *---------------------------------------------------------------------------*/
typedef struct {
    GMutex       mutex;    /* protects the struct (init-on-first-use)       */
    GtkTextView *view;     /* NULL until bound                              */
} UStudioLogState;

static UStudioLogState g_log_state = { 0 };

/*-----------------------------------------------------------------------------
 * Append a line to the GtkTextView (must run on GTK main thread).
 *---------------------------------------------------------------------------*/
static void append_to_view(GtkTextView *tv, const char *line) {
    if (!tv || !line) return;
    GtkTextBuffer *buf = gtk_text_view_get_buffer(tv);
    if (!buf) return;

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    gtk_text_buffer_insert(buf, &end, line, -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);
}

/*-----------------------------------------------------------------------------
 * Idle trampoline: runs on the GTK main loop. The payload is a g_strdup'd
 * string that we free after appending.
 *---------------------------------------------------------------------------*/
typedef struct {
    char *line;
} IdleLine;

static gboolean idle_append_line(gpointer data) {
    IdleLine *p = (IdleLine*)data;
    /* Snapshot target under lock to avoid races */
    GtkTextView *tv = NULL;
    g_mutex_lock(&g_log_state.mutex);
    tv = g_log_state.view;
    g_mutex_unlock(&g_log_state.mutex);

    if (p && p->line) {
        if (tv) append_to_view(tv, p->line);
        /* Always mirror to GLib log for redundancy */
        g_message("%s", p->line);
    }
    if (p) g_free(p->line);
    g_free(p);
    return G_SOURCE_REMOVE;
}

/*-----------------------------------------------------------------------------
 * Public: bind a GtkTextView as the UI log target (may be NULL to detach).
 * Safe to call multiple times and from any thread.
 *---------------------------------------------------------------------------*/
void ustudio_logging_init(GtkTextView *optional_view) {
    g_mutex_lock(&g_log_state.mutex);
    g_log_state.view = optional_view;
    g_mutex_unlock(&g_log_state.mutex);
}

/*-----------------------------------------------------------------------------
 * Public: log a single line. Thread-safe; posts to main loop.
 *---------------------------------------------------------------------------*/
void ustudio_log_line(const char *line) {
    if (!line) return;
    IdleLine *p = g_new0(IdleLine, 1);
    p->line = g_strdup(line);
    /* Post to GTK main loop; no C++ lambda here, just a named function. */
    g_idle_add(idle_append_line, p);
}

/*-----------------------------------------------------------------------------
 * Public: log with printf-style formatting. Thread-safe.
 *---------------------------------------------------------------------------*/
void ustudio_log_fmt(const char *fmt, ...) {
    if (!fmt) return;
    va_list ap;
    va_start(ap, fmt);
    char *s = g_strdup_vprintf(fmt, ap);
    va_end(ap);

    IdleLine *p = g_new0(IdleLine, 1);
    p->line = s;
    g_idle_add(idle_append_line, p);
}

/*-----------------------------------------------------------------------------
 * OPTIONAL: GLib log writer (commented out by default).
 * If you want to capture g_message()/g_warning() streams into the view,
 * uncomment ustudio_install_glib_writer() and call it from startup.
 *---------------------------------------------------------------------------*/
#if 0
static GLogWriterOutput writer(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data) {
    const char *msg = NULL;
    for (gsize i = 0; i < n_fields; i++) {
        if (g_strcmp0(fields[i].key, "MESSAGE") == 0) {
            msg = (const char*)fields[i].value;
            break;
        }
    }
    if (msg) ustudio_log_line(msg);
    return G_LOG_WRITER_HANDLED;
}
void ustudio_install_glib_writer(void) {
    g_log_set_writer_func(writer, NULL, NULL);
}
#endif

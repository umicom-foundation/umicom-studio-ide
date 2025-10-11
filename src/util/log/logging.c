/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File:  src/util/log/logging.c
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

#include "include/logging.h"        /* Public API we implement */
#include "include/console_logger.h" /* To mirror into the text console pane  */
#include "include/status_util.h"    /* For status-bar mirroring              */

#include <stdarg.h>                 /* va_list for fmt helper */
#include <string.h>                 /* strlen used in comments */
#include <stdio.h>                  /* vsnprintf (via g_strdup_vprintf)      */

/* Global sinks; guarded by a mutex so worker threads may also log safely. */
static GMutex        g_log_mutex;        /* Protects sink pointers */
static GtkTextView  *g_log_view = NULL;  /* Borrowed; owned by UI */
static UmiStatus    *g_status   = NULL;  /* Borrowed; owned by UI */

/* Internal: push a single message into UI + status bar (runs on main thread). */
static void
log_to_ui_and_status(const char *msg)
{
    /* Mirror to the console text view if bound. */
    if (g_log_view) {
        ustudio_console_log_line(msg);          /* Appends to bound view with newline */
    }
    /* Also mirror into the status bar if bound (non-destructive). */
    if (g_status) {
        umi_status_push(g_status, msg);         /* Show latest message briefly */
    }
}

/* Idle payload so worker threads can safely request UI updates. */
typedef struct {
    char *line;                                 /* Owned UTF-8 string */
} IdleLine;

static gboolean
idle_append_line(gpointer data)
{
    IdleLine *p = (IdleLine *)data;             /* Take ownership of payload */
    log_to_ui_and_status(p->line);              /* Update UI sinks from main loop */
    g_free(p->line);                            /* Free message buffer */
    g_free(p);                                  /* Free payload */
    return G_SOURCE_REMOVE;                     /* One-shot idle handler */
}

/* Public: bind the status bar mirror. */
void
ustudio_log_bind(UmiStatus *status)
{
    g_mutex_lock(&g_log_mutex);                 /* Serialize updates to sinks */
    g_status = status;                          /* Borrowed pointer */
    g_mutex_unlock(&g_log_mutex);
}

/* GLib writer: receives log records produced by g_message/g_warning/etc.
 * We convert fields to a concise single-line message and dispatch it. */
static GLogWriterOutput
writer_func(GLogLevelFlags   log_level,
            const GLogField *fields,
            gsize            n_fields,
            gpointer         user_data)
{
    (void) user_data;                           /* Unused (required signature) */

    const char *msg = NULL;                     /* Pointer to message text */
    const char *dom = NULL;                     /* Optional log domain */
    for (gsize i = 0; i < n_fields; ++i) {
        if (g_strcmp0(fields[i].key, "MESSAGE") == 0) {
            msg = (const char *)fields[i].value;/* GLib provides MESSAGE as UTF-8 */
        } else if (g_strcmp0(fields[i].key, "GLIB_DOMAIN") == 0) {
            dom = (const char *)fields[i].value;/* Optional domain */
        }
    }

    if (!msg) msg = "";                         /* Be robust if MESSAGE is missing */

    /* Compose a compact line: "[domain] message" (domain omitted if NULL). */
    gchar *line = dom && *dom
                ? g_strdup_printf("[%s] %s", dom, msg)
                : g_strdup(msg);

    /* Always print to stderr for developers (keeps console logs during CI). */
    g_printerr("%s\n", line);

    /* If there is a UI sink, dispatch via idle so worker threads are safe. */
    g_mutex_lock(&g_log_mutex);
    const gboolean have_ui = (g_log_view != NULL);
    g_mutex_unlock(&g_log_mutex);

    if (have_ui) {
        IdleLine *payload = g_new0(IdleLine, 1);   /* Allocate payload */
        payload->line = line;                      /* Transfer ownership to idle */
        g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, idle_append_line, payload, NULL);
    } else {
        g_free(line);                           /* No UI sink: free locally */
    }

    /* Let GLib know we handled the record; don't fall back to default. */
    return G_LOG_WRITER_HANDLED;
}

void
ustudio_logging_init(GtkTextView *output_view)
{
    /* Keep both our direct pointer (for status and view checks) and the separate
     * console_logger binding (used by the console pane implementation). */
    g_mutex_lock(&g_log_mutex);
    g_log_view = output_view;                       /* Borrowed pointer */
    g_mutex_unlock(&g_log_mutex);

    /* Bind the console pane helper so ustudio_console_log_line() knows the view. */
    if (output_view) {
        ustudio_console_log_bind(output_view);
    }

    /* Install our writer so GLib logs are routed to UI/status as well. */
    g_log_set_writer_func(writer_func, NULL, NULL);
}

void
ustudio_log_line(const char *msg)
{
    if (!msg) return;                                /* Nothing to log */

    /* Mirror to stderr for developers right away. */
    g_printerr("%s\n", msg);

    /* If we have UI sinks, queue into the GTK main loop so it remains thread-safe. */
    g_mutex_lock(&g_log_mutex);
    const gboolean have_ui = (g_log_view != NULL);
    g_mutex_unlock(&g_log_mutex);

    if (have_ui) {
        IdleLine *payload = g_new0(IdleLine, 1);
        payload->line = g_strdup(msg);              /* Copy so caller can free/modify its own string */
        g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, idle_append_line, payload, NULL);
    }
}

void
ustudio_log_fmt(const char *fmt, ...)
{
    if (!fmt) return;                                /* No format, nothing to do */

    va_list ap;
    va_start(ap, fmt);                               /* Begin var-args */
    gchar *line = g_strdup_vprintf(fmt, ap);         /* Allocate formatted string */
    va_end(ap);                                      /* End var-args */

    ustudio_log_line(line);                          /* Reuse single-line pathway */
    g_free(line);                                    /* Free temporary buffer */
}

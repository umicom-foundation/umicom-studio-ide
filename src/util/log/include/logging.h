/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/

#ifndef USTUDIO_LOGGING_H
#define USTUDIO_LOGGING_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Bind GLib logs (g_message, g_warning, g_error (fatal), g_debug) to a GtkTextView. */
void ustudio_log_bind(GtkTextView *view);

G_END_DECLS
#endif

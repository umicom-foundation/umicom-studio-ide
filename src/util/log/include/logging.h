/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/include/logging.h
 * PURPOSE: Central logging setup + helpers (GLib writer -> UI console/status)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_LOGGING_H
#define UMICOM_LOGGING_H

#include <glib.h>   /* G_GNUC_PRINTF, GLogWriterOutput */
#include <gtk/gtk.h>/* GtkTextView */

#include "status_util.h"   /* UmiStatus */

/**
 * ustudio_logging_init:
 * @output_view: GtkTextView used as the console sink; may be NULL to disable UI sink
 *
 * Installs a GLib log writer that routes g_message()/g_warning()/etc. into
 * our UI console (if bound) and also prints to stderr for developer visibility.
 */
void ustudio_logging_init(GtkTextView *output_view);

/**
 * ustudio_log_line:
 * @msg: UTF-8 message to log (newline is added automatically for the UI pane)
 *
 * Thread-safe entry point that mirrors a single message into the UI console and
 * updates the bound status bar (if any). Equivalent to g_message() for simple use.
 */
void ustudio_log_line(const char *msg);

/**
 * ustudio_log_fmt:
 * @fmt: printf-style format
 * @...: values
 *
 * Convenience formatting wrapper over ustudio_log_line(). Marked printf-like.
 */
void ustudio_log_fmt(const char *fmt, ...) G_GNUC_PRINTF(1, 2);

/**
 * ustudio_log_bind:
 * @status: Optional UmiStatus wrapper for mirroring last message into the status bar
 *
 * If provided, each logged message also updates the status bar text.
 */
void ustudio_log_bind(UmiStatus *status);

#endif /* UMICOM_LOGGING_H */
/* End of src/util/log/include/logging.h */
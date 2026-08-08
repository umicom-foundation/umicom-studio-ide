/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/include/console_logger.h
 * PURPOSE: Minimal text-view backed console logger (UI output pane)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_CONSOLE_LOGGER_H
#define UMICOM_CONSOLE_LOGGER_H

#include <glib.h>  /* gboolean, NULL */
#include <gtk/gtk.h> /* GtkTextView */

/**
 * ustudio_console_log_bind:
 * @output_view: GtkTextView instance that owns/logs text lines (must outlive binding)
 *
 * Binds the logger to a GtkTextView so later calls to ustudio_console_log_line()
 * append text into it. Safe to call multiple times; last call wins.
 */
void ustudio_console_log_bind(GtkTextView *output_view);

/**
 * ustudio_console_log_line:
 * @line: UTF-8 string to append (a trailing newline is added by the function)
 *
 * Appends one line to the bound GtkTextView, if any. No-op if not bound yet.
 * UI-thread only (call from GTK main thread). If you may be on a worker thread,
 * use g_idle_add() to proxy the call to the main loop.
 */
void ustudio_console_log_line(const char *line);

#endif /* UMICOM_CONSOLE_LOGGER_H */
/* End of src/util/log/include/console_logger.h */
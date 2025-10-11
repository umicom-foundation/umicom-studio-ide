/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ansi_color.h
 * PURPOSE: Minimal ANSI SGR parser that writes into GtkTextBuffer with tags
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ANSI_COLOR_H
#define UMICOM_ANSI_COLOR_H

#include <gtk/gtk.h>

typedef struct _UmiAnsi UmiAnsi;

/* Create a colorizer bound to a GtkTextBuffer.
 * It pre-creates a handful of common tags: bold, italic, underline,
 * and colors: red, yellow, green, blue, magenta, cyan, gray. */
UmiAnsi *umi_ansi_new(GtkTextBuffer *buf);
void     umi_ansi_free(UmiAnsi *a);

/* Append a line with ANSI escapes (\\n optional). */
void     umi_ansi_append_line(UmiAnsi *a, const char *ansi_text);

/* Utility: ensure tags exist on the buffer (safe to call multiple times). */
void     umi_ansi_ensure_tags(GtkTextBuffer *buf);

#endif /* UMICOM_ANSI_COLOR_H */

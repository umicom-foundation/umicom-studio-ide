/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_filters.h
 * PURPOSE: Chainable output filters (timestamp, severity)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OUTPUT_FILTERS_H
#define UMICOM_OUTPUT_FILTERS_H

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

/* Opaque handle; implementation can evolve without touching callers. */
typedef struct _UmiAnsi UmiAnsi;

/* Create a filter that writes into the provided GtkTextBuffer. */
UmiAnsi *umi_ansi_new(GtkTextBuffer *buf);

/* Append one logical line. ANSI CSI sequences are stripped for now. */
void umi_ansi_append_line(UmiAnsi *a, const char *line);

/* Destroy the filter and release any allocated resources. */
void umi_ansi_free(UmiAnsi *a);

G_END_DECLS
#endif /* UMICOM_OUTPUT_FILTERS_H */

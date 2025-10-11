/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_filters.h
 * PURPOSE: Chainable output filters (timestamp, severity)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OUTPUT_FILTERS_H
#define UMICOM_OUTPUT_FILTERS_H

#include <glib.h>

typedef struct _UmiOutLine {
  gchar *text;       /* line text (no trailing newline) */
  GHashTable *meta;  /* string->string */
} UmiOutLine;

typedef gboolean (*UmiOutFilter)(UmiOutLine *line, gpointer user);

typedef struct {
  GPtrArray *filters; /* of UmiOutFilter */
  GPtrArray *users;   /* of gpointer */
} UmiOutChain;

/* Create/destroy */
UmiOutChain *umi_out_chain_new(void);
void         umi_out_chain_free(UmiOutChain *c);

/* Add a filter to the end (executed in order). */
void         umi_out_chain_add(UmiOutChain *c, UmiOutFilter fn, gpointer user);

/* Process a line in-place. Returns FALSE if a filter consumes/drops it. */
gboolean     umi_out_chain_process(UmiOutChain *c, UmiOutLine *line);

/* Utilities: predefined filters */
gboolean umi_out_filter_timestamp(UmiOutLine *line, gpointer user); /* adds meta: ts */
gboolean umi_out_filter_severity(UmiOutLine *line, gpointer user);  /* adds meta: sev=info|warn|error */

#endif /* UMICOM_OUTPUT_FILTERS_H */

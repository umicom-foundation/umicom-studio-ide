/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/telemetry.c
 * PURPOSE: In-memory counters with GHashTable (no persistence)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/telemetry.h"

static GHashTable *g_counts = NULL;

void umi_tel_inc(const char *counter){
  if(!g_counts) g_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  if(!counter) return;
  gpointer v = g_hash_table_lookup(g_counts, counter);
  guint64 x = (v ? GPOINTER_TO_UINT(v) : 0) + 1;
  g_hash_table_insert(g_counts, g_strdup(counter), GUINT_TO_POINTER((guint)x));
}

guint64 umi_tel_get(const char *counter){
  if(!g_counts || !counter) return 0;
  return (guint64)GPOINTER_TO_UINT(g_hash_table_lookup(g_counts, counter));
}

void umi_tel_reset_all(void){
  if(!g_counts) return;
  g_hash_table_remove_all(g_counts);
}

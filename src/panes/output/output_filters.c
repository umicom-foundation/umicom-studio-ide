/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_filters.c
 * PURPOSE: Implementation of output filter chain
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "output_filters.h"
#include "timestamp.h"

UmiOutChain *umi_out_chain_new(void){
  UmiOutChain *c = g_new0(UmiOutChain,1);
  c->filters = g_ptr_array_new();
  c->users   = g_ptr_array_new();
  return c;
}

void umi_out_chain_free(UmiOutChain *c){
  if(!c) return;
  g_ptr_array_free(c->filters, TRUE);
  g_ptr_array_free(c->users, TRUE);
  g_free(c);
}

void umi_out_chain_add(UmiOutChain *c, UmiOutFilter fn, gpointer user){
  if(!c || !fn) return;
  g_ptr_array_add(c->filters, (gpointer)fn);
  g_ptr_array_add(c->users, user);
}

gboolean umi_out_chain_process(UmiOutChain *c, UmiOutLine *line){
  if(!c || !line) return TRUE;
  for(guint i=0;i<c->filters->len;i++){
    UmiOutFilter fn = (UmiOutFilter)c->filters->pdata[i];
    gpointer u = c->users->pdata[i];
    if(!fn(line, u)) return FALSE;
  }
  return TRUE;
}

static void meta_set(UmiOutLine *l, const char *k, const char *v){
  if(!l->meta) l->meta = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  g_hash_table_insert(l->meta, g_strdup(k), g_strdup(v));
}

gboolean umi_out_filter_timestamp(UmiOutLine *line, gpointer user){
  (void)user;
  gchar *ts = umi_now_iso8601();
  meta_set(line, "ts", ts);
  g_free(ts);
  return TRUE;
}

gboolean umi_out_filter_severity(UmiOutLine *line, gpointer user){
  (void)user;
  const char *s = line->text ? line->text : "";
  const char *sev = "info";
  if(g_strrstr(s, "error:") || g_strrstr(s, "ERROR") || g_strrstr(s, "fail")) sev = "error";
  else if(g_strrstr(s, "warning:") || g_strrstr(s, "WARN")) sev = "warn";
  meta_set(line, "sev", sev);
  return TRUE;
}

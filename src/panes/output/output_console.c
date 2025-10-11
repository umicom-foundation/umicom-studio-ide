/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_console.c
 * PURPOSE: Implementation of rich console
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/output_console.h"

struct _UmiOutputConsole {
  GtkWidget *root;
  GtkWidget *view;
  GtkTextBuffer *buf;
  UmiAnsi *ansi;
  UmiOutChain *chain;
};

UmiOutputConsole* umi_output_console_new(void){
  UmiOutputConsole *c = g_new0(UmiOutputConsole,1);
  c->buf = gtk_text_buffer_new(NULL);
  c->ansi = umi_ansi_new(c->buf);
  c->chain = umi_out_chain_new();
  umi_out_chain_add(c->chain, umi_out_filter_timestamp, NULL);
  umi_out_chain_add(c->chain, umi_out_filter_severity,  NULL);
  c->view = gtk_text_view_new_with_buffer(c->buf);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(c->view), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(c->view), TRUE);
  GtkWidget *scr = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), c->view);
  c->root = scr;
  return c;
}

GtkWidget* umi_output_console_widget(UmiOutputConsole *c){ return c?c->root:NULL; }
void umi_output_console_clear(UmiOutputConsole *c){ if(!c) return; gtk_text_buffer_set_text(c->buf, "", -1); }
UmiOutChain* umi_output_console_chain(UmiOutputConsole *c){ return c?c->chain:NULL; }

void umi_output_console_append_line(UmiOutputConsole *c, const char *line){
  if(!c || !line) return;
  UmiOutLine L = {0};
  L.text = (char*)line;
  if(!umi_out_chain_process(c->chain, &L)) return;
  /* Prepend timestamp & severity into text (visible hint), then colorize original line */
  const char *sev = L.meta ? (const char*)g_hash_table_lookup(L.meta, "sev") : NULL;
  const char *ts  = L.meta ? (const char*)g_hash_table_lookup(L.meta, "ts")  : NULL;
  gchar *prefix = g_strdup_printf("[%s] %s: ", ts?ts:"", sev?sev:"info");
  GtkTextIter end; gtk_text_buffer_get_end_iter(c->buf, &end);
  gtk_text_buffer_insert(c->buf, &end, prefix, -1);
  g_free(prefix);
  umi_ansi_append_line(c->ansi, line);
  if(L.meta) g_hash_table_unref(L.meta);
}

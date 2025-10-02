/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search_panel.c
 * PURPOSE: Implementation of the search panel using ripgrep
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "search_panel.h"

/* This helper adds a line to the result list. The UI uses a GtkListBox
 * because it is the simplest way to show many uniform rows. */
static void add_row(GtkListBox *lb, const char *text){
  GtkWidget *row = gtk_label_new(text?text:"");
  gtk_label_set_xalign(GTK_LABEL(row), 0.0);
  gtk_list_box_append(lb, row);
}

static void on_clicked(GtkButton *b, gpointer user){
  (void)b;
  UmiSearchPanel *p = (UmiSearchPanel*)user;
  const char *pat = gtk_editable_get_text(GTK_EDITABLE(p->entry));
  if(!pat || !*pat){ if(p->status) umi_status_flash(p->status, "Type something to search", 1200); return; }

  UmiRgOpts o = {0}; o.pattern = (char*)pat; o.root = "."; o.ignore_case = TRUE;
  GError *e=NULL; GSubprocess *sub = umi_rg_spawn(&o, &e);
  if(!sub){
    if(p->status) umi_status_set(p->status, e?e->message:"ripgrep failed");
    if(e) g_error_free(e);
    return;
  }

  /* Read all output at once (simplest path for a demo). A production version
   * should stream line-by-line and keep the UI responsive for very large outputs. */
  gchar *out=NULL; gsize len=0;
  g_subprocess_communicate_utf8(sub, NULL, NULL, &out, NULL, &e);
  if(e){ if(p->status) umi_status_set(p->status, e->message); g_error_free(e); }
  if(out){
    /* Split by lines and show the first 200 results to keep the demo snappy. */
    gchar **lines = g_strsplit(out, "\n", -1);
    for(int i=0; lines[i] && i<200; ++i) if(lines[i][0]) add_row(p->list, lines[i]);
    g_strfreev(lines);
    g_free(out);
  }
  g_object_unref(sub);
}

UmiSearchPanel *umi_search_panel_new(UmiStatus *status){
  UmiSearchPanel *p = g_new0(UmiSearchPanel,1);
  p->status = status;
  p->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  p->entry = GTK_ENTRY(gtk_entry_new());
  gtk_editable_set_text(GTK_EDITABLE(p->entry), "main|error|warning");
  p->btn = GTK_BUTTON(gtk_button_new_with_label("Search"));
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(p->entry));
  gtk_box_append(GTK_BOX(row), GTK_WIDGET(p->btn));
  gtk_box_append(GTK_BOX(p->root), row);

  p->list = GTK_LIST_BOX(gtk_list_box_new());
  gtk_box_append(GTK_BOX(p->root), GTK_WIDGET(p->list));

  g_signal_connect(p->btn, "clicked", G_CALLBACK(on_clicked), p);
  return p;
}

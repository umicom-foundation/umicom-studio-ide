/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/command_palette.c
 * PURPOSE: Implementation of a minimal, filterable palette
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "command_palette.h"
#include "utf8.h"

struct _UmiPalette {
  GtkWidget *dlg;
  GtkWidget *entry;
  GtkWidget *list;
  const UmiCommand *table;
  gpointer user;
};

static void populate(UmiPalette *p, const char *filter){
  GList *children = gtk_container_get_children(GTK_CONTAINER(p->list));
  for(GList *l=children;l;l=l->next) gtk_container_remove(GTK_CONTAINER(p->list), GTK_WIDGET(l->data));
  g_list_free(children);

  for(const UmiCommand *c=p->table; c && c->name; ++c){
    if(filter && *filter){
      gchar *combo = g_strdup_printf("%s %s", c->name, c->hint?c->hint:"");
      gboolean ok = umi_utf8_icontains(combo, filter);
      g_free(combo);
      if(!ok) continue;
    }
    GtkWidget *btn = gtk_button_new_with_label(c->name);
    g_object_set_data(G_OBJECT(btn), "umi_cb", (gpointer)c->cb);
    g_signal_connect_swapped(btn, "clicked", G_CALLBACK(c->cb), p->user);
    gtk_box_append(GTK_BOX(p->list), btn);
  }
}

static void on_changed(GtkEditable *e, gpointer u){
  UmiPalette *p = (UmiPalette*)u;
  const char *q = gtk_editable_get_text(e);
  populate(p, q);
}

UmiPalette *umi_palette_new(const UmiCommand *table, gpointer user){
  UmiPalette *p = g_new0(UmiPalette,1);
  p->table = table; p->user = user;

  p->dlg = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(p->dlg), "Command Palette");
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(p->dlg));
  p->entry = gtk_entry_new();
  p->list  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_box_append(GTK_BOX(content), p->entry);
  gtk_box_append(GTK_BOX(content), p->list);
  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);
  populate(p, "");
  return p;
}

void umi_palette_open(UmiPalette *p, GtkWindow *parent){
  if(!p) return;
  gtk_window_set_transient_for(GTK_WINDOW(p->dlg), parent);
  gtk_window_present(GTK_WINDOW(p->dlg));
}

void umi_palette_free(UmiPalette *p){
  if(!p) return;
  gtk_window_destroy(GTK_WINDOW(p->dlg));
  g_free(p);
}

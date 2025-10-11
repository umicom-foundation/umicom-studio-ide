/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/palette.c
 * PURPOSE: Implementation of a tiny command palette demo
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "palette.h"

static void populate(UmiPalette *p, const char *filter){
  gtk_list_box_remove_all(p->list);
  for(guint i=0;i<p->cmds->len;i++){
    UmiCmd *c = (UmiCmd*)p->cmds->pdata[i];
    if(!filter || umi_utf8_icontains(c->name, filter)){
      GtkWidget *row = gtk_button_new_with_label(c->name);
      g_signal_connect_swapped(row, "clicked", G_CALLBACK(c->fn), c->user);
      gtk_list_box_append(p->list, row);
    }
  }
}

static void on_changed(GtkEditable *e, gpointer user){
  UmiPalette *p = (UmiPalette*)user;
  const char *txt = gtk_editable_get_text(e);
  populate(p, txt);
}

UmiPalette *umi_palette_new(GtkWindow *parent){
  UmiPalette *p = g_new0(UmiPalette,1);
  p->cmds = g_ptr_array_new();

  p->dialog = gtk_dialog_new_with_buttons("Command Palette", parent, 0, "Close", GTK_RESPONSE_CLOSE, NULL);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  p->entry = GTK_ENTRY(gtk_entry_new());
  p->list  = GTK_LIST_BOX(gtk_list_box_new());
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(p->entry));
  gtk_box_append(GTK_BOX(box), GTK_WIDGET(p->list));
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(p->dialog));
  gtk_box_append(GTK_BOX(content), box);

  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);
  return p;
}

void umi_palette_set_commands(UmiPalette *p, GPtrArray *cmds){
  if(!p) return;
  p->cmds = cmds?cmds:p->cmds;
  populate(p, NULL);
}

void umi_palette_open(UmiPalette *p){
  if(!p) return;
  gtk_window_present(GTK_WINDOW(p->dialog));
}

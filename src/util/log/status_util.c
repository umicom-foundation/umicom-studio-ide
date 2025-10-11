/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_util.c
 * PURPOSE: Implementation of status helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "status_util.h"

static gboolean restore_cb(gpointer d){
  UmiStatus *s = (UmiStatus*)d;
  (void)s; /* We restore to current label text already set elsewhere; left as a stub for now. */
  return G_SOURCE_REMOVE;
}

UmiStatus *umi_status_new(void){
  UmiStatus *s = g_new0(UmiStatus,1);
  s->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  s->label = GTK_LABEL(gtk_label_new(""));
  gtk_box_append(GTK_BOX(s->root), GTK_WIDGET(s->label));
  return s;
}

GtkWidget *umi_status_widget(UmiStatus *s){ return s? s->root : NULL; }

void umi_status_set(UmiStatus *s, const char *text){
  if(!s) return;
  gtk_label_set_text(s->label, text?text:"");
}

void umi_status_flash(UmiStatus *s, const char *text, guint msec){
  if(!s) return;
  gtk_label_set_text(s->label, text?text:"");
  g_timeout_add(msec?msec:1200, restore_cb, s);
}

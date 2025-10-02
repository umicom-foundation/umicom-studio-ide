/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_bar.c
 * PURPOSE: Implementation of status bar composite
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "status_bar.h"

typedef struct _UmiStatusBar {
  GtkWidget *root;
  GtkLabel  *label;
  GtkSpinner *spin;
  gchar *permanent;
} UmiStatusBar;

UmiStatusBar *umi_status_bar_new(void){
  UmiStatusBar *sb = g_new0(UmiStatusBar,1);
  sb->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  sb->label = GTK_LABEL(gtk_label_new(""));
  sb->spin  = GTK_SPINNER(gtk_spinner_new());
  gtk_box_append(GTK_BOX(sb->root), GTK_WIDGET(sb->spin));
  gtk_box_append(GTK_BOX(sb->root), GTK_WIDGET(sb->label));
  return sb;
}

GtkWidget *umi_status_bar_widget(UmiStatusBar *sb){ return sb?sb->root:NULL; }

void umi_status_bar_set(UmiStatusBar *sb, const char *text){
  if(!sb) return;
  g_free(sb->permanent);
  sb->permanent = g_strdup(text?text:"");
  gtk_label_set_text(sb->label, sb->permanent);
}

static gboolean restore_cb(gpointer d){
  UmiStatusBar *sb = (UmiStatusBar*)d;
  if(!sb) return G_SOURCE_REMOVE;
  gtk_label_set_text(sb->label, sb->permanent?sb->permanent:"");
  return G_SOURCE_REMOVE;
}

void umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint msec){
  if(!sb) return;
  gtk_label_set_text(sb->label, text?text:"");
  g_timeout_add(msec?msec:1500, restore_cb, sb);
}

void umi_status_bar_busy(UmiStatusBar *sb, gboolean busy){
  if(!sb) return;
  if(busy) gtk_spinner_start(sb->spin); else gtk_spinner_stop(sb->spin);
}

void umi_status_bar_free(UmiStatusBar *sb){
  if(!sb) return;
  g_free(sb->permanent);
  g_free(sb);
}

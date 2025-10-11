/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/toolbar.c
 * PURPOSE: Implementation of toolbar with basic actions
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/toolbar.h"

struct _UmiToolbar {
  GtkWidget *root;
  GtkWidget *btn_run, *btn_stop, *btn_save, *btn_palette;
  UmiToolbarCallbacks cb;
};

static GtkWidget *mk_btn(const char *txt, UmiNoArgCb cb, gpointer user){
  GtkWidget *b = gtk_button_new_with_label(txt);
  if(cb) g_signal_connect_swapped(b, "clicked", G_CALLBACK(cb), user);
  return b;
}

UmiToolbar *umi_toolbar_new(const UmiToolbarCallbacks *cb){
  UmiToolbar *t = g_new0(UmiToolbar,1);
  if(cb) t->cb = *cb;
  t->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  t->btn_run     = mk_btn("Run",     t->cb.on_run,     t->cb.user);
  t->btn_stop    = mk_btn("Stop",    t->cb.on_stop,    t->cb.user);
  t->btn_save    = mk_btn("Save",    t->cb.on_save,    t->cb.user);
  t->btn_palette = mk_btn("Palette", t->cb.on_palette, t->cb.user);
  gtk_box_append(GTK_BOX(t->root), t->btn_run);
  gtk_box_append(GTK_BOX(t->root), t->btn_stop);
  gtk_box_append(GTK_BOX(t->root), t->btn_save);
  gtk_box_append(GTK_BOX(t->root), t->btn_palette);
  return t;
}

GtkWidget *umi_toolbar_widget(UmiToolbar *tb){ return tb?tb->root:NULL; }

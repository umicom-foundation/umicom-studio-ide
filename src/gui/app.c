/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app.c
 * PURPOSE: Implementation of top-level application shell
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>

#include "gui/app.h"

__attribute__((weak)) void umi_run_pipeline_start(gpointer out, gpointer problems, gpointer reserved);
__attribute__((weak)) void umi_run_pipeline_stop(void);
__attribute__((weak)) void umi_editor_save(UmiEditor *ed, GError **error);

static GHashTable *g_map = NULL;

static void do_run(gpointer u){
  (void)u;
  if (umi_run_pipeline_start) umi_run_pipeline_start(NULL, NULL, NULL);
}
static void do_stop(gpointer u){
  (void)u;
  if (umi_run_pipeline_stop) umi_run_pipeline_stop();
}
static void do_save(gpointer u){
  UmiApp *ua = (UmiApp*)u;
  if (umi_editor_save && ua) { GError *e=NULL; umi_editor_save(ua->ed, &e); if(e) g_error_free(e); }
}
static GtkWidget* make_toolbar(UmiApp *ua){
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *btn_run = gtk_button_new_with_label("Run");
  GtkWidget *btn_stop = gtk_button_new_with_label("Stop");
  GtkWidget *btn_save = gtk_button_new_with_label("Save");
  gtk_box_append(GTK_BOX(box), btn_run);
  gtk_box_append(GTK_BOX(box), btn_stop);
  gtk_box_append(GTK_BOX(box), btn_save);
  g_signal_connect_swapped(btn_run,  "clicked", G_CALLBACK(do_run),  ua);
  g_signal_connect_swapped(btn_stop, "clicked", G_CALLBACK(do_stop), ua);
  g_signal_connect_swapped(btn_save, "clicked", G_CALLBACK(do_save), ua);
  return box;
}
static void on_activate(GtkApplication *app, gpointer user){
  (void)user;
  if(!g_map) g_map = g_hash_table_new(g_direct_hash, g_direct_equal);
  UmiApp *ua = g_hash_table_lookup(g_map, app);
  if(!ua){ ua = g_new0(UmiApp,1); ua->app = app; g_hash_table_insert(g_map, app, ua); }
  ua->win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(ua->win, "Umicom Studio IDE");
  gtk_window_set_default_size(ua->win, 1200, 800);

  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(root), make_toolbar(ua));

  GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(GTK_BOX(root), hpaned);

  GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_box_append(GTK_BOX(left), gtk_label_new("File Tree"));
  gtk_paned_set_start_child(GTK_PANED(hpaned), left);

  GtkWidget *editor_widget = gtk_label_new("Editor");
  gtk_paned_set_end_child(GTK_PANED(hpaned), editor_widget);

  GtkWidget *status = gtk_label_new("Status");
  gtk_box_append(GTK_BOX(root), status);
  gtk_window_set_child(ua->win, root);
  gtk_window_present(ua->win);
}
static void on_startup(GtkApplication *app, gpointer user){
  (void)app; (void)user;
  if(!g_map) g_map = g_hash_table_new(g_direct_hash, g_direct_equal);
}
GtkApplication *umi_app_new(void){
  GtkApplication *app = gtk_application_new("org.umicom.studio", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect(app, "startup",  G_CALLBACK(on_startup),  NULL);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return app;
}
UmiApp *umi_app_handle(GtkApplication *app){
  if(!g_map) return NULL;
  return g_hash_table_lookup(g_map, app);
}
GtkWindow *umi_app_window(UmiApp *ua){ return ua?ua->win:NULL; }
UmiEditor *umi_app_editor(UmiApp *ua){ return ua?ua->ed:NULL; }

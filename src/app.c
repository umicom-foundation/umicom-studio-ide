/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app.c
 * PURPOSE: Implementation of top-level application shell
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "app.h"
#include "toolbar.h"
#include "workspace.h"
#include "menu.h"
#include "actions.h"
#include "keymap.h"
#include "theme.h"
#include "status_util.h"
#include "run_pipeline.h"

typedef struct { UmiApp base; } UmiAppImpl;
static GHashTable *g_map = NULL; /* GtkApplication* -> UmiApp* */

static void on_tree_open(gpointer u, const char *path){
  UmiApp *ua = (UmiApp*)u;
  if(!ua || !ua->ed) return;
  GError *e=NULL; umi_editor_open_file(ua->ed, path, &e); if(e) g_error_free(e);
}

static void do_run(gpointer u){ (void)u; umi_run_pipeline_start(((UmiApp*)u)->ed->out, ((UmiApp*)u)->ed->problems, NULL); }
static void do_stop(gpointer u){ (void)u; umi_run_pipeline_stop(); }
static void do_save(gpointer u){ UmiApp *ua=(UmiApp*)u; GError *e=NULL; umi_editor_save(ua->ed, &e); if(e) g_error_free(e); }
static void do_palette(gpointer u){ (void)u; /* Palette is created in actions.c in our menus stack */ }

static void on_activate(GtkApplication *app, gpointer user){
  (void)user;
  UmiApp *ua = g_hash_table_lookup(g_map, app);
  if(!ua){
    ua = g_new0(UmiApp,1);
    ua->app = app;
    g_hash_table_insert(g_map, app, ua);
  }

  ua->win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(ua->win, "Umicom Studio IDE");
  gtk_window_set_default_size(ua->win, 1200, 800);

  /* Core widgets */
  ua->ed   = umi_editor_new();
  ua->tree = umi_file_tree_new(on_tree_open, ua);
  ua->search = umi_search_panel_new(NULL, NULL);
  ua->status = umi_status_bar_new();

  /* Layout: sidebar + editor + bottom status */
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *hbox = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  GtkWidget *vleft = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

  /* Toolbar */
  UmiToolbarCallbacks tcb = { .on_run=do_run, .on_stop=do_stop, .on_save=do_save, .on_palette=do_palette, .user=ua };
  UmiToolbar *tb = umi_toolbar_new(&tcb);
  gtk_box_append(GTK_BOX(root), umi_toolbar_widget(tb));

  gtk_paned_set_start_child(GTK_PANED(hbox), vleft);
  gtk_box_append(GTK_BOX(vleft), umi_file_tree_widget(ua->tree));
  gtk_paned_set_end_child(GTK_PANED(hbox), GTK_WIDGET(ua->ed->root));
  gtk_box_append(GTK_BOX(root), hbox);
  gtk_box_append(GTK_BOX(root), umi_status_bar_widget(ua->status));
  gtk_window_set_child(ua->win, root);

  /* Menus + actions + keymap */
  umi_actions_install(app);
  UmiKeymapCallbacks km = {0}; km.user = app; /* filled in actions layer */
  umi_keymap_install(ua->win, &km);

  /* Theme default */
  umi_theme_apply(ua->win, "light");

  gtk_window_present(ua->win);
}

static void on_startup(GtkApplication *app, gpointer user){
  (void)user;
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

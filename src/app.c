/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app.c
 * PURPOSE: Implementation of top-level application shell
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include "app.h"
#include "app_menu_llm.h"
#include "editor.h"
#include "status_bar.h"
#include "search_panel.h"
#include "keymap.h"
#include "app_actions.h"

static void open_file(UmiApp *ua, const char *path){
  if(!ua || !ua->ed || !path) return;
  GError *e=NULL;
  umi_editor_open_file(ua->ed, path, &e);
  if(e){ g_error_free(e); }
}

static void do_save(gpointer u){
  UmiApp *ua=(UmiApp*)u; GError *e=NULL; umi_editor_save(ua->ed, &e); if(e) g_error_free(e);
}

UmiApp *umi_app_new(GtkApplication *app){
  UmiApp *ua = g_new0(UmiApp,1);
  ua->app = app;

  /* Window + root */
  ua->win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(ua->win, "Umicom Studio IDE");
  gtk_window_set_default_size(ua->win, 1100, 700);

  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(ua->win, root);

  /* Menu */
  GtkWidget *menubar = umi_app_menu_llm_new(do_save, ua);
  gtk_box_append(GTK_BOX(root), menubar);

  /* Split: file tree (future) | editor */
  GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_hexpand(paned, TRUE);
  gtk_widget_set_vexpand(paned, TRUE);
  gtk_box_append(GTK_BOX(root), paned);

  /* Editor */
  ua->ed = umi_editor_new();
  GtkWidget *edw = umi_editor_widget(ua->ed);
  gtk_paned_set_end_child(GTK_PANED(paned), edw);

  /* Status bar wrapper */
  ua->status = umi_status_bar_new();
  gtk_box_append(GTK_BOX(root), umi_status_bar_widget(ua->status));

  /* Search panel uses the inner UmiStatus* (avoids type mismatch warnings) */
  ua->search = umi_search_panel_new(umi_status_bar_inner(ua->status));
  /* pack search panel somewhere when ready, or leave detached for now. */

  /* Keyboard shortcuts */
  UmiKeymapCallbacks km = {0};
  km.user = ua;
  umi_app_fill_keymap(app, &km);
  umi_keymap_install(ua->win, &km);

  return ua;
}

GtkWidget *umi_app_widget(UmiApp *ua){
  return ua ? GTK_WIDGET(ua->win) : NULL;
}

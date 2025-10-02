/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_tree.c
 * PURPOSE: Implementation: minimal non-virtualized directory tree
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "file_tree.h"
#include <string.h>

typedef struct _Node {
  gchar *path;
  gboolean is_dir;
} Node;

struct _UmiFileTree {
  GtkWidget *root;
  GtkWidget *view;
  GtkTreeStore *store; /* columns: path (string), is_dir (bool), name (string) */
  UmiFileActivateCb on_act;
  gpointer user;
  gchar *root_dir;
};

enum { COL_PATH, COL_IS_DIR, COL_NAME, N_COLS };

static void clear_store(UmiFileTree *t){
  gtk_tree_store_clear(t->store);
}

static void add_dir(UmiFileTree *t, GtkTreeIter *parent, const char *dir){
  GDir *d = g_dir_open(dir, 0, NULL);
  if(!d) return;
  const gchar *name;
  while((name = g_dir_read_name(d))){
    if(name[0]=='.') continue;
    gchar *path = g_build_filename(dir, name, NULL);
    gboolean is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);
    GtkTreeIter it;
    gtk_tree_store_append(t->store, &it, parent);
    gtk_tree_store_set(t->store, &it,
      COL_PATH, path,
      COL_IS_DIR, is_dir,
      COL_NAME, name,
      -1);
    if(is_dir) add_dir(t, &it, path);
    g_free(path);
  }
  g_dir_close(d);
}

static void refresh(UmiFileTree *t){
  if(!t || !t->root_dir) return;
  clear_store(t);
  GtkTreeIter rootit;
  gtk_tree_store_append(t->store, &rootit, NULL);
  gtk_tree_store_set(t->store, &rootit, COL_PATH, t->root_dir, COL_IS_DIR, TRUE, COL_NAME, t->root_dir, -1);
  add_dir(t, &rootit, t->root_dir);
  gtk_tree_view_expand_all(GTK_TREE_VIEW(t->view));
}

static void on_row_activated(GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *col, gpointer u){
  (void)col;
  UmiFileTree *t = (UmiFileTree*)u; GtkTreeIter it;
  if(gtk_tree_model_get_iter(GTK_TREE_MODEL(t->store), &it, path)){
    gchar *p=NULL; gboolean is_dir=FALSE;
    gtk_tree_model_get(GTK_TREE_MODEL(t->store), &it, COL_PATH, &p, COL_IS_DIR, &is_dir, -1);
    if(p && !is_dir && t->on_act) t->on_act(t->user, p);
    g_free(p);
  }
}

UmiFileTree *umi_file_tree_new(UmiFileActivateCb on_activate, gpointer user){
  UmiFileTree *t = g_new0(UmiFileTree,1);
  t->on_act = on_activate; t->user = user;
  t->store = gtk_tree_store_new(N_COLS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING);
  t->view  = gtk_tree_view_new_with_model(GTK_TREE_MODEL(t->store));
  GtkCellRenderer *r = gtk_cell_renderer_text_new();
  gtk_tree_view_append_column(GTK_TREE_VIEW(t->view),
    gtk_tree_view_column_new_with_attributes("Name", r, "text", COL_NAME, NULL));
  g_signal_connect(t->view, "row-activated", G_CALLBACK(on_row_activated), t);
  t->root = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(t->root), t->view);
  return t;
}

GtkWidget *umi_file_tree_widget(UmiFileTree *t){ return t? t->root : NULL; }

void umi_file_tree_set_root(UmiFileTree *t, const char *root_dir){
  if(!t) return;
  g_free(t->root_dir);
  t->root_dir = g_strdup(root_dir?root_dir:".");
  refresh(t);
}

void umi_file_tree_refresh(UmiFileTree *t){ refresh(t); }

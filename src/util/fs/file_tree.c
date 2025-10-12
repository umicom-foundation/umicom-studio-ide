/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/file_tree.c
 * PURPOSE: Minimal GtkTreeView-based file tree
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "file_tree.h"              /* Public API */

#include <string.h>                         /* strcmp for sorting */

/* Columns in our GtkTreeStore model. */
enum {
  COL_NAME = 0,                             /* Visible text: file/folder name only. */
  COL_PATH,                                 /* Full path (for activation / actions). */
  COL_IS_DIR,                               /* gboolean flag: TRUE if directory. */
  N_COLS
};

/* Internal object structure (opaque to callers). */
struct _UmiFileTree {
  GtkTreeStore      *store;                  /* Backing store for the tree view. */
  GtkTreeView       *view;                   /* The widget the caller packs. */
  gchar             *root;                   /* Current root directory (g_strdup). */
  UmiFileActivateCb  on_activate;            /* Row-activation callback. */
  gpointer           user;                   /* Caller cookie for callbacks. */
};

/* -------------------------- Forward declarations -------------------------- */
static void clear_store(UmiFileTree *t);                       /* Wipe all rows. */
static void add_dir(UmiFileTree *t, GtkTreeIter *parent_iter, const char *dir); /* Recursively add rows. */
static void rebuild(UmiFileTree *t);                           /* Clear + add. */
static void on_row_activated(GtkTreeView *v, GtkTreePath *tp, GtkTreeViewColumn *col, gpointer user);
/* ------------------------------------------------------------------------- */

/* Create a new file tree and its widget. */
UmiFileTree *umi_file_tree_new(UmiFileActivateCb on_activate, gpointer user){
  UmiFileTree *t = g_new0(UmiFileTree, 1);                     /* Zeroed allocation. */

  t->store = gtk_tree_store_new(N_COLS,                        /* Create model with our columns: */
                                G_TYPE_STRING,                 /* COL_NAME: displayed text */
                                G_TYPE_STRING,                 /* COL_PATH: absolute/full path */
                                G_TYPE_BOOLEAN);               /* COL_IS_DIR: dir flag */

  t->view  = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(t->store))); /* View bound to model. */
  t->on_activate = on_activate;                                /* Save callback pointer. */
  t->user = user;                                              /* Save user cookie. */

  /* Add a single text column for names. */
  GtkCellRenderer *r = gtk_cell_renderer_text_new();           /* Renders plain text. */
  GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes("Name", r, "text", COL_NAME, NULL);
  gtk_tree_view_append_column(t->view, c);                     /* Show the column on the view. */

  /* Fire callback when a row is activated (double-click/Enter). */
  g_signal_connect(t->view, "row-activated", G_CALLBACK(on_row_activated), t);

  return t;                                                    /* Return fully-constructed object. */
}

/* Expose the widget to the caller for packing. */
GtkWidget *umi_file_tree_widget(UmiFileTree *t){
  return t ? GTK_WIDGET(t->view) : NULL;                       /* Return view (or NULL). */
}

/* Change the displayed root directory and rebuild the tree. */
void umi_file_tree_set_root(UmiFileTree *t, const char *path){
  if(!t) return;                                               /* Guard against NULL. */
  g_free(t->root);                                             /* Drop previous root (if any). */
  t->root = path ? g_strdup(path) : NULL;                      /* Copy new root string. */
  rebuild(t);                                                  /* Rebuild the entire tree from new root. */
}

/* Public refresh: rebuild from current root. */
void umi_file_tree_refresh(UmiFileTree *t){
  if(!t) return;                                               /* Safe no-op on NULL. */
  rebuild(t);                                                  /* Rebuild (clear + fill) from t->root. */
}

/* Free all resources. */
void umi_file_tree_free(UmiFileTree *t){
  if(!t) return;                                               /* Safe no-op on NULL. */
  if(t->store) g_object_unref(t->store);                       /* Unref model. */
  if(t->view)  g_object_unref(t->view);                        /* Unref view widget. */
  g_free(t->root);                                             /* Free root string. */
  g_free(t);                                                   /* Free the struct. */
}

/* ------------------------------ Internals -------------------------------- */

/* Recursively add a directory's entries to the tree under 'parent_iter' (or as roots if NULL). */
static void add_dir(UmiFileTree *t, GtkTreeIter *parent_iter, const char *dir){
  if(!t || !dir) return;                                       /* Validate input. */

  GDir *d = g_dir_open(dir, 0, NULL);                          /* Attempt to open the directory. */
  if(!d) return;                                               /* Can't open -> stop at this branch. */

  /* Buffer names for stable sorting. */
  GPtrArray *names = g_ptr_array_new_with_free_func(g_free);   /* Owns each strdup'd name. */
  const gchar *name;                                           /* Iteration variable (non-owned). */
  while((name = g_dir_read_name(d))){                          /* Grab all entries first. */
    if(name[0]=='.')                                           /* Skip hidden entries. */
      continue;
    g_ptr_array_add(names, g_strdup(name));                    /* Keep a copy to sort later. */
  }
  g_dir_close(d);                                              /* Done reading. */

  g_ptr_array_sort_with_data(names, (GCompareDataFunc)g_strcmp0, NULL); /* Sort for determinism. */

  /* Emit rows for each child. */
  for(guint i=0;i<names->len;i++){
    const char *n = (const char*)names->pdata[i];              /* Borrowed string pointer. */
    gchar *path = g_build_filename(dir, n, NULL);              /* Compute full path. */
    gboolean is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);   /* Check if folder. */

    GtkTreeIter child;                                         /* Iterator representing the new row. */
    gtk_tree_store_append(t->store, &child, parent_iter);      /* Append under parent (or as root). */
    gtk_tree_store_set(t->store, &child,
                       COL_NAME, n,                             /* Display name (leaf text). */
                       COL_PATH, path,                          /* Full path for activation. */
                       COL_IS_DIR, is_dir,                      /* Whether it’s a directory. */
                       -1);                                     /* List terminator. */

    if(is_dir){                                                /* Recurse into subdirectories. */
      /* Prevent naive symlink loops. */
      if(!g_file_test(path, G_FILE_TEST_IS_SYMLINK))
        add_dir(t, &child, path);
    }

    g_free(path);                                              /* Free path allocated by g_build_filename. */
  }

  g_ptr_array_free(names, TRUE);                               /* Free name list (and elements). */
}

/* Clear the entire model. */
static void clear_store(UmiFileTree *t){
  gtk_tree_store_clear(t->store);                              /* Clear all rows from the model. */
}

/* Clear and rebuild tree from t->root. */
static void rebuild(UmiFileTree *t){
  clear_store(t);                                              /* Always start from a clean model. */
  if(!t->root || !*t->root)                                    /* If no root set... */
    return;                                                    /* ...leave the tree empty. */
  if(!g_file_test(t->root, G_FILE_TEST_IS_DIR))                /* If root isn't a directory... */
    return;                                                    /* ...we only show directories in this view. */
  add_dir(t, NULL, t->root);                                   /* Populate starting from root level. */
}

/* Row activation handler: fetch path + dir flag and invoke caller callback. */
static void on_row_activated(GtkTreeView *v, GtkTreePath *tp, GtkTreeViewColumn *col, gpointer user){
  (void)col;                                                   /* Unused parameter: column is irrelevant here. */
  UmiFileTree *t = (UmiFileTree*)user;                         /* Our instance pointer. */

  GtkTreeModel *m = gtk_tree_view_get_model(v);                /* Borrow the model. */
  GtkTreeIter it;                                              /* Iterator for the activated row. */
  if(!gtk_tree_model_get_iter(m, &it, tp))                     /* Resolve the path to an iterator. */
    return;                                                    /* If it fails, nothing to do. */

  gchar *path = NULL;                                          /* Will receive g_malloced path string. */
  gboolean is_dir = FALSE;                                     /* Will receive boolean flag. */
  gtk_tree_model_get(m, &it,
                     COL_PATH, &path,                          /* Extract stored path for the row. */
                     COL_IS_DIR, &is_dir,                      /* Extract stored directory flag. */
                     -1);                                      /* Terminator. */

  if(t->on_activate)                                           /* If the user provided a callback... */
    t->on_activate(t->user, path, is_dir);                     /* ...invoke it with data. */

  g_free(path);                                                /* Free string returned by gtk_tree_model_get. */
}

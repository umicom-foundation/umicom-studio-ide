/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/util/fs/file_tree.c
 *
 * PURPOSE:
 *   Provide a tiny, self‑contained file tree widget based on GtkTreeView.
 *   This module exposes a very small API (see file_tree.h) so the rest of
 *   the IDE can display a navigable directory tree and react when users
 *   activate a row (double‑click/Enter).
 *
 * SCOPE:
 *   - Build the model from a given root directory.
 *   - Rebuild on demand (refresh).
 *   - Notify caller via a callback when a row is activated.
 *
 * IMPORTANT DESIGN NOTES:
 *   - We keep the implementation simple and synchronous. If the tree is
 *     pointed at a very large folder, callers should build it off the GTK
 *     main loop (e.g., in a worker thread) and then hand the model back.
 *   - GtkTreeView/GtkTreeStore are deprecated in favor of GtkListView and
 *     GtkTreeListModel in newer GTK. Migrating is planned, but for now we
 *     keep the existing UI and *silence deprecation warnings* for this file
 *     using GLib’s G_GNUC_BEGIN/END_IGNORE_DEPRECATIONS to keep builds clean.
 *   - No dependencies on IDE output panes or other UI modules—this remains
 *     a small, reusable utility.
 *
 * THREADING:
 *   - All functions are synchronous; not thread‑safe unless guarded by the
 *     caller. GTK widgets should only be touched on the main thread.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include "file_tree.h"     /* Public API: struct Opaque + callback typedefs       */
#include <glib.h>          /* GDir/GPtrArray utilities                            */
#include <glib/gstdio.h>   /* g_file_test utilities                               */
#include <string.h>        /* g_strcmp0                                            */

/*------------------------------- Model columns ---------------------------------
 * We store three columns:
 *   COL_NAME  : leaf filename shown in the UI
 *   COL_PATH  : full, absolute path (used when the user activates a row)
 *   COL_IS_DIR: gboolean flag (directory or regular file)
 *-----------------------------------------------------------------------------*/
enum {
  COL_NAME = 0,
  COL_PATH,
  COL_IS_DIR,
  N_COLS
};

/*-------------------------- Internal object layout ----------------------------
 * The concrete struct is private to this translation unit. The public header
 * holds only an opaque pointer typedef (`typedef struct _UmiFileTree UmiFileTree;`).
 *-----------------------------------------------------------------------------*/
struct _UmiFileTree {
  GtkTreeStore      *store;          /* tree model (rows + columns)                 */
  GtkTreeView       *view;           /* widget presented to callers                  */
  gchar             *root;           /* g_strdup() of the current root directory     */
  UmiFileActivateCb  on_activate;    /* activation callback provided by the caller   */
  gpointer           user;           /* user data cookie echoed on callbacks         */
};

/* Forward declarations of local helpers. */
static void         clear_store (UmiFileTree *t);
static void         add_dir     (UmiFileTree *t, GtkTreeIter *parent_iter, const char *dir);
static void         rebuild     (UmiFileTree *t);
static void         on_row_activated(GtkTreeView *v, GtkTreePath *tp, GtkTreeViewColumn *col, gpointer user);

/* Comparator wrapper matching GCompareDataFunc for strings. */
static gint cmp_cstrings(gconstpointer a, gconstpointer b, gpointer user_data){
  (void)user_data;                           /* unused */
  const char *sa = (const char *)a;
  const char *sb = (const char *)b;
  return g_strcmp0(sa, sb);
}

/*------------------------------- Public API ----------------------------------*/
UmiFileTree *umi_file_tree_new(UmiFileActivateCb on_activate, gpointer user)
{
  /* Silence deprecation warnings within this block – we keep the legacy
   * TreeView/TreeStore for now to avoid churning the UI layer. */
#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  UmiFileTree *t = g_new0(UmiFileTree, 1);

  /* Create the model with our three columns. */
  t->store = gtk_tree_store_new(N_COLS,
                                G_TYPE_STRING,   /* COL_NAME  */
                                G_TYPE_STRING,   /* COL_PATH  */
                                G_TYPE_BOOLEAN); /* COL_IS_DIR*/

  /* Bind a TreeView to the model. */
  t->view  = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(t->store)));

  /* Add a single text column: “Name”. */
  GtkCellRenderer   *r = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes("Name", r, "text", COL_NAME, NULL);
  gtk_tree_view_append_column(t->view, c);

  /* Hook row‑activation to our trampoline. */
  g_signal_connect(t->view, "row-activated", G_CALLBACK(on_row_activated), t);
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif

  /* Save callback + cookie. */
  t->on_activate = on_activate;
  t->user        = user;
  return t;
}

/* Return the widget for packing. */
GtkWidget *umi_file_tree_widget(UmiFileTree *t){
#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  return t ? GTK_WIDGET(t->view) : NULL;
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif
}

/* Replace root directory and rebuild. */
void umi_file_tree_set_root(UmiFileTree *t, const char *path){
  if(!t) return;
  g_free(t->root);
  t->root = path ? g_strdup(path) : NULL;
  rebuild(t);
}

/* Rebuild from current root. */
void umi_file_tree_refresh(UmiFileTree *t){
  if(!t) return;
  rebuild(t);
}

/* Destroy widget/model and free memory. */
void umi_file_tree_free(UmiFileTree *t){
  if(!t) return;
#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  if(t->store) g_object_unref(t->store);
  if(t->view)  g_object_unref(t->view);
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif
  g_clear_pointer(&t->root, g_free);
  g_free(t);
}

/*------------------------------- Internals -----------------------------------*/
/* Append a directory’s children (files + subdirs) under parent_iter (or root). */
static void add_dir(UmiFileTree *t, GtkTreeIter *parent_iter, const char *dir){
  if(!t || !dir) return;

  GError *err = NULL;
  GDir   *d   = g_dir_open(dir, 0, &err);
  if(!d){
    g_clear_error(&err);
    return;
  }

  /* Read all names, sort for deterministic ordering, then emit rows. */
  GPtrArray *names = g_ptr_array_new_with_free_func(g_free);
  const gchar *name;
  while((name = g_dir_read_name(d))){
    if(name[0]=='.') continue;              /* hide dot‑files in tree view */
    g_ptr_array_add(names, g_strdup(name)); /* keep a copy for sorting     */
  }
  g_dir_close(d);

  g_ptr_array_sort_with_data(names, cmp_cstrings, NULL);

#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  for(guint i=0;i<names->len;i++){
    const char *leaf = (const char*)names->pdata[i];
    gchar *full      = g_build_filename(dir, leaf, NULL);
    const gboolean is_dir = g_file_test(full, G_FILE_TEST_IS_DIR);

    GtkTreeIter child;
    gtk_tree_store_append(t->store, &child, parent_iter);
    gtk_tree_store_set(t->store, &child,
                       COL_NAME,   leaf,
                       COL_PATH,   full,
                       COL_IS_DIR, is_dir,
                       -1);

    if(is_dir){
      /* Avoid naive symlink loops. */
      if(!g_file_test(full, G_FILE_TEST_IS_SYMLINK))
        add_dir(t, &child, full);
    }
    g_free(full);
  }
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif
  g_ptr_array_free(names, TRUE);
}

/* Clear the model completely. */
static void clear_store(UmiFileTree *t){
#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  gtk_tree_store_clear(t->store);
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif
}

/* Clear + rebuild the model from t->root (directories only for now). */
static void rebuild(UmiFileTree *t){
  if(!t) return;
  clear_store(t);
  if(!t->root || !*t->root) return;
  if(!g_file_test(t->root, G_FILE_TEST_IS_DIR)) return;
  add_dir(t, NULL, t->root);
}

/* Called when a row is double‑clicked or Enter is pressed. */
static void on_row_activated(GtkTreeView *v, GtkTreePath *tp, GtkTreeViewColumn *col, gpointer user){
  (void)col;                     /* column pointer not used */
  UmiFileTree *t = (UmiFileTree*)user;

#if defined(G_GNUC_BEGIN_IGNORE_DEPRECATIONS)
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
#endif
  GtkTreeModel *m = gtk_tree_view_get_model(v);
  GtkTreeIter it;
  if(!gtk_tree_model_get_iter(m, &it, tp))
    return;

  gchar *path = NULL;
  gboolean is_dir = FALSE;
  gtk_tree_model_get(m, &it,
                     COL_PATH,   &path,
                     COL_IS_DIR, &is_dir,
                     -1);
#if defined(G_GNUC_END_IGNORE_DEPRECATIONS)
  G_GNUC_END_IGNORE_DEPRECATIONS
#endif

  if(t->on_activate)
    t->on_activate(t->user, path, is_dir);

  g_free(path);
}
/*--------------------------------- End of file -------------------------------*/

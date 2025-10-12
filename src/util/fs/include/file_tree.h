/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/file_tree.h
 * PURPOSE: Minimal GTK file tree widget wrapper
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_TREE_H
#define UMICOM_FILE_TREE_H

#include <gtk/gtk.h>     /* GtkTreeView, GtkTreeStore, etc. */
#include <glib.h>        /* gchar, gboolean */

typedef struct _UmiFileTree UmiFileTree;  /* Opaque handle. */

/* Callback when a row (file/dir) is activated from the UI. */
typedef void (*UmiFileActivateCb)(gpointer user, const char *path, gboolean is_dir);

/* Construct a new file tree.
   - 'on_activate' is invoked when the user activates a row (double-click/Enter).
   - 'user' is passed back to the callback untouched. */
UmiFileTree *umi_file_tree_new(UmiFileActivateCb on_activate, gpointer user);

/* Get the GTK widget to pack into your UI. */
GtkWidget   *umi_file_tree_widget(UmiFileTree *t);

/* Change the root directory displayed by the tree and reload. */
void         umi_file_tree_set_root(UmiFileTree *t, const char *path);

/* Re-scan current root and refresh the view. */
void         umi_file_tree_refresh(UmiFileTree *t);

/* Free all resources held by the tree (safe on NULL). */
void         umi_file_tree_free(UmiFileTree *t);

#endif /* UMICOM_FILE_TREE_H */

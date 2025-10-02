/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_tree.h
 * PURPOSE: File tree widget with set_root/refresh + activate callbacks
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_TREE_H
#define UMICOM_FILE_TREE_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmiFileTree UmiFileTree;

/* User chooses a file; we send absolute path. */
typedef void (*UmiFileActivateCb)(gpointer user, const char *path);

/* Construct a file tree; 'on_activate' may be NULL. */
UmiFileTree *umi_file_tree_new(UmiFileActivateCb on_activate, gpointer user);

/* Set or change the root directory; refreshes immediately. */
void         umi_file_tree_set_root(UmiFileTree *t, const char *root_dir);

/* Force a refresh. */
void         umi_file_tree_refresh(UmiFileTree *t);

/* Access widget for packing. */
GtkWidget   *umi_file_tree_widget(UmiFileTree *t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_FILE_TREE_H */

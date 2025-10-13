/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/workspace.h
 * PURPOSE: Workspace root management and persistence
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKSPACE_H
#define UMICOM_WORKSPACE_H

#include <gtk/gtk.h>
#include "file_tree.h"

typedef struct _UmiWorkspace {
  gchar *root_dir;
  UmiFileTree *tree;
} UmiWorkspace;

/* Create workspace bound to an existing file tree (may be NULL). */
UmiWorkspace *umi_workspace_new(UmiFileTree *tree);

/* Load last root from config/workspace.json and set it. */
void umi_workspace_restore(UmiWorkspace *ws);

/* Set root (persists to config/workspace.json) and refresh tree. */
void umi_workspace_set_root(UmiWorkspace *ws, const char *dir);

/* Get current root (owned by workspace). */
const char *umi_workspace_root(UmiWorkspace *ws);

/* Destroy and free. */
void umi_workspace_free(UmiWorkspace *ws);

#endif /* UMICOM_WORKSPACE_H */

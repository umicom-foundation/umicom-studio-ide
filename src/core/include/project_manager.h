/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/project_manager.h
 * PURPOSE: High-level project/open workspace management API
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROJECT_MANAGER_H
#define UMICOM_PROJECT_MANAGER_H

#include <gtk/gtk.h>
#include "workspace.h"
#include "../../util/fs/include/file_index.h"
#include "recent_files.h"
#include "../../util/log/include/status_util.h"

/* The Project Manager is a tiny facade that collects common actions a contributor
 * needs when first opening Umicom Studio. The comments below explain *every* member:
 *
 * - ws:      pointer to the Workspace model (owning the root folder path).
 * - index:   cached recursive listing of files under the workspace root.
 * - recent:  MRU list shared with the rest of the app; adding to it keeps the
 *            welcome screen up-to-date.
 * - status:  human-readable status line where we tell the contributor what we did.
 */
typedef struct {
  UmiWorkspace *ws;
  UmiFileIndex *index;
  UmiRecent    *recent;
  UmiStatus    *status;
} UmiProjectManager;

/* Create a new project manager by wiring the existing subsystems. */
UmiProjectManager *umi_project_manager_new(UmiWorkspace *ws, UmiRecent *recent, UmiStatus *status);

/* Open a folder as the workspace root. This *also* triggers a re-index so that
 * the file tree shows up immediately for new contributors. */
gboolean umi_project_open(UmiProjectManager *pm, const char *folder, GError **err);

/* Refresh the file index, useful after cloning or changing branches. */
void     umi_project_refresh_index(UmiProjectManager *pm);

/* Free allocated resources (index only; the shared singletons are owned elsewhere). */
void     umi_project_manager_free(UmiProjectManager *pm);

#endif /* UMICOM_PROJECT_MANAGER_H */

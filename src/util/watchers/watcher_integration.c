/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_integration.c
 * PURPOSE: Implementation of watcher glue – connects the recursive file
 *          watcher to higher-level workspace + file-tree refresh.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/watcher_integration.h"  /* Public API for UmiWatchGlue, workspace & tree types */

/*-----------------------------------------------------------------------------
 * Internal callback: invoked by the recursive watcher when something changes.
 *
 * Parameters:
 *   u    - Opaque user data we provided when starting the watcher (UmiWatchGlue*)
 *   path - Path for the changed item (may be NULL or unused here)
 *
 * Behavior:
 *   - Safely cast the opaque pointer to our glue struct.
 *   - If we still have a live tree, request a refresh.
 *   - We intentionally avoid using "path" here because the tree-level refresh
 *     wants a global update; (void)path prevents an "unused parameter" warning.
 *   - IMPORTANT: If the lower-level watcher calls us from a worker thread, the
 *     implementation of umi_file_tree_refresh() must marshal to GTK's main
 *     thread internally. If it does not, add a main-loop dispatch here.
 *---------------------------------------------------------------------------*/
static void on_evt(gpointer u, const char *path)
{
  (void)path;                               /* Explicitly mark 'path' unused here */

  UmiWatchGlue *g = (UmiWatchGlue*)u;       /* Cast opaque pointer back to our type */
  if (!g) return;                           /* Defensive: nothing to do if glue is NULL */

  if (g->tree) {                            /* Only act if we still hold a valid tree ref */
    /* Ask the file-tree to refresh its view of the workspace filesystem. */
    umi_file_tree_refresh(g->tree);
  }
}

/*-----------------------------------------------------------------------------
 * umi_watchglue_start
 *
 * PURPOSE:
 *   Build the glue object that ties the workspace root to the recursive
 *   watcher. Registers the above callback so UI reacts to fs changes.
 *
 * Parameters:
 *   ws   - Workspace handle (must not be NULL)
 *   tree - File-tree UI/controller to refresh on changes (must not be NULL)
 *
 * Returns:
 *   New UmiWatchGlue* on success; NULL on failure or bad inputs.
 *
 * Notes:
 *   - We only allocate minimal state here: a small glue struct and the
 *     recursive watcher. Ownership is clear: caller stops via umi_watchglue_stop().
 *   - We fall back to "." if workspace root is absent (defensive behavior).
 *---------------------------------------------------------------------------*/
UmiWatchGlue *umi_watchglue_start(UmiWorkspace *ws, UmiFileTree *tree)
{
  if (!ws || !tree) {
    /* Invalid inputs – refuse to start, avoid partial state. */
    return NULL;
  }

  /* Allocate and zero-initialize the glue struct. Using g_new0 ensures fields start at 0/NULL. */
  UmiWatchGlue *g = g_new0(UmiWatchGlue, 1);

  /* Keep references (not owning) to the workspace and file-tree supplied by the caller. */
  g->ws   = ws;
  g->tree = tree;

  /* Determine the filesystem root to monitor; use "." as a safe default. */
  const char *root = umi_workspace_root(ws);
  const char *watch_root = (root && *root) ? root : ".";

  /* Create the recursive watcher:
   *  - watch_root: base directory to observe
   *  - on_evt:     our callback (above) for any change
   *  - g:          user data passed back to the callback
   */
  g->rec = umi_watchrec_new(watch_root, on_evt, g);

  /* If watcher creation failed, release the glue and report failure. */
  if (!g->rec) {
    g_free(g);
    return NULL;
  }

  /* Success – return the live glue object to the caller. */
  return g;
}

/*-----------------------------------------------------------------------------
 * umi_watchglue_stop
 *
 * PURPOSE:
 *   Stop the recursive watcher and free the glue object created by
 *   umi_watchglue_start().
 *
 * Parameters:
 *   g - Glue object to stop (may be NULL; if so, this is a no-op)
 *
 * Behavior:
 *   - Shuts down the watcher if present.
 *   - Frees the glue struct itself.
 *   - Caller must drop any pointers to 'g' after calling this.
 *---------------------------------------------------------------------------*/
void umi_watchglue_stop(UmiWatchGlue *g)
{
  if (!g) return;              /* Nothing to do */

  if (g->rec) {                /* Stop and destroy the recursive watcher first */
    umi_watchrec_free(g->rec);
    g->rec = NULL;
  }

  /* Finally free the glue struct. */
  g_free(g);
}

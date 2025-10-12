/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_integration.c
 * PURPOSE:
 *   Implementation of watcher ↔ UI glue.
 *
 *   Minimal, compile-safe implementation:
 *   - Owns a non-recursive path watcher.
 *   - On any event, asks the file tree to refresh.
 *   - Keeps the API surface stable so we can later expand to recursive and/or
 *    
* workspace-driven paths without touching callers.
* * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

/* Public API for this unit */
#include "watcher_integration.h"

/* Path watcher API (opaque handle + add/stop/free) */
#include "path_watcher.h"

/* File tree API: we need the refresh function + the type name. The include
 * path for this header is already on your compile command:
 *   -I .../src/util/fs/include
 */
#include "file_tree.h"

/*-----------------------------------------------------------------------------
 * Private glue object
 *---------------------------------------------------------------------------*/
struct _UmiWatchGlue {
  UmiFileTree    *tree;      /* UI tree to refresh on FS changes             */
  UmiPathWatcher *pw;        /* Non-recursive watcher handle (may be NULL)   */
  /* Future: add recursive watcher, per-workspace roots, filters, etc.        */
};

/*-----------------------------------------------------------------------------
 * FS event callback: refresh the file tree when something changes.
 * GIO dispatches on the main context, so we can call directly into UI helpers.
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
static void
on_evt(gpointer u, const char *path)
{
  (void)path; /* not used yet; keep signature to allow richer UIs later */

  UmiWatchGlue *g = (UmiWatchGlue *)u;
  if (!g) return;

  if (g->tree) {
    /* Ask the tree to rebuild/refresh its view of the file system. */
    umi_file_tree_refresh(g->tree);
  }
}

/*-----------------------------------------------------------------------------
 * Start glue: create watcher + hook callback.
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

 *---------------------------------------------------------------------------*/
UmiWatchGlue *
umi_watchglue_start(UmiWorkspace *ws, UmiFileTree *tree)
{
  (void)ws; /* Unused in this minimal integration. */

  UmiWatchGlue *g = g_new0(UmiWatchGlue, 1);
  g->tree = tree;

   /* Determine the filesystem root to monitor; use "." as a safe default. */
  const char *root = umi_workspace_root(ws);
  const char *watch_root = (root && *root) ? root : ".";
  /* Create a path watcher and bind our callback. We deliberately don’t add
   * any directories here; higher layers can call umi_pathwatch_add() with
   * workspace/project roots when appropriate. This keeps us build-safe now.
   */
  g->pw = umi_pathwatch_new(on_evt, g);
   /* If watcher creation failed, release the glue and report failure. */
  if (!g->rec) {
    g_free(g);
    return NULL;
  }

  /* Success – return the live glue object to the caller. */
  return g;
}

/*-----------------------------------------------------------------------------
 * Stop glue: tear down watchers and free the glue object.
*-----------------------------------------------------------------------------
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
  if (!g) return;

  if (g->pw) {
    /* Detach and free watcher resources. */
    umi_pathwatch_stop(g->pw);
    umi_pathwatch_free(g->pw);
    g->pw = NULL;
  }

 /* Finally free the glue struct. */
  g_free(g);
}
/*--- end of file ---*/
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_integration.c
 * PURPOSE:
 *   Implementation of the integration layer that connects recursive file
 *   watchers to our UI’s FileTree and (optionally) Workspace state.
 *
 * DESIGN:
 *   - Own a UmiWatcherRec and route its callback to refresh the FileTree.
 *   - No heavy logic here; it’s a “glue” module by design.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "watcher_integration.h"

#include <gio/gio.h>
#include <glib.h>

/* These are intentionally forward-declared in the header to keep includes light. */
struct _UmiWatcherIntegration {
    FileTree       *tree;   /* Borrowed; not owned */
    WorkspaceState *ws;     /* Borrowed; optional; not used here yet */
    UmiWatcherRec  *rec;    /* Owned; created on first add */
};

/* External function from the FileTree module. We declare here to avoid heavy
 * includes. This matches your existing symbol used in other code paths. */
void umi_file_tree_refresh(FileTree *tree);

/*-----------------------------------------------------------------------------
 * Callback from the recursive watcher
 *---------------------------------------------------------------------------*/
static void on_evt(gpointer u, const char *path)
{
    (void)path; /* For now we refresh everything; future: selective updates */

    UmiWatcherIntegration *g = (UmiWatcherIntegration*)u;
    if (!g || !g->tree) return;

    /* Keep it small & safe: schedule a refresh; if you require marshaling
     * into the GTK main context, do it here with g_idle_add_full(). */
    umi_file_tree_refresh(g->tree);
}

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/
UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws)
{
    UmiWatcherIntegration *wi = g_new0(UmiWatcherIntegration, 1);
    wi->tree = tree;
    wi->ws   = ws;
    wi->rec  = NULL; /* lazy creation on first add */
    return wi;
}

gboolean umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req)
{
    if (!wi || !req || !req->path || !*req->path) return FALSE;

    /* Lazily create a recursive watcher on first use; the initial root is the
     * first added path. Subsequent calls add more roots/dirs. */
    if (!wi->rec) {
        /* If the request points to a file, umi_watchrec_new() will still work;
         * it will resolve the parent dir before attaching monitors. */
        wi->rec = umi_watchrec_new(req->path, on_evt, wi);
        if (!wi->rec) return FALSE;
    } else {
        if (!umi_watchrec_add(wi->rec, req->path))
            return FALSE;
    }

    /* We don’t need to special-case req->recursive here because the recursive
     * watcher always monitors subdirectories by construction. */
    (void)req->recursive;

    return TRUE;
}

void umi_watch_integ_free(UmiWatcherIntegration *wi)
{
    if (!wi) return;
    if (wi->rec) {
        umi_watchrec_free(wi->rec);
        wi->rec = NULL;
    }
    /* 'tree' and 'ws' are borrowed; do not free. */
    g_free(wi);
}

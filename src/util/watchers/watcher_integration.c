/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_integration.c
 *
 * PURPOSE:
 *   Integration layer: owns a recursive watcher and routes its callback to
 *   refresh UI components (FileTree), optionally touching Workspace state.
 *
 * DESIGN:
 *   - Very thin by design; keep work minimal in callback.
 *   - Future: debounce/throttle refresh via g_timeout_add if needed.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "watcher_integration.h"
#include <gio/gio.h>
#include <glib.h>

/* Forward declare UI refresh to avoid heavy includes. */
void umi_file_tree_refresh(struct _FileTree *tree);

struct _UmiWatcherIntegration {
    FileTree       *tree;   /* borrowed */
    WorkspaceState *ws;     /* borrowed (unused for now) */
    UmiWatcherRec  *rec;    /* owned */
};

static void on_evt(gpointer u, const char *path)
{
    (void)path; /* refresh whole tree; future: selective refresh */
    UmiWatcherIntegration *g = (UmiWatcherIntegration*)u;
    if (!g || !g->tree) return;

    /* If you must marshal to GTK main thread explicitly, use g_idle_add(). */
    umi_file_tree_refresh(g->tree);
}

UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws)
{
    UmiWatcherIntegration *wi = g_new0(UmiWatcherIntegration, 1);
    wi->tree = tree;
    wi->ws   = ws;
    wi->rec  = NULL;
    return wi;
}

gboolean umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req)
{
    if (!wi || !req || !req->path || !*req->path) return FALSE;

    if (!wi->rec) {
        wi->rec = umi_watchrec_new(req->path, on_evt, wi);
        if (!wi->rec) return FALSE;
    } else {
        if (!umi_watchrec_add(wi->rec, req->path)) return FALSE;
    }
    (void)req->recursive; /* recursive by construction */
    return TRUE;
}

void umi_watch_integ_free(UmiWatcherIntegration *wi)
{
    if (!wi) return;
    if (wi->rec) { umi_watchrec_free(wi->rec); wi->rec = NULL; }
    g_free(wi);
}

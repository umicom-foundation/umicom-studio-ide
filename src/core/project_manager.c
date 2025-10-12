/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/project_manager.c
 * PURPOSE: Minimal project manager matching the current public header
 *          (project_manager.h): create/free/open/refresh with no UI/status
 *          dependency and no assumptions about Workspace/Index APIs.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------
 * Quick Start / Notes
 * - This implementation strictly follows the header:
 *
 *     typedef struct {
 *       UmiWorkspace *ws;
 *       UmiFileIndex *index;
 *       UmiRecent    *recent;
 *       UmiStatus    *status;
 *     } UmiProjectManager;
 *
 *     UmiProjectManager *umi_project_manager_new(UmiWorkspace *ws,
 *                                                UmiRecent *recent,
 *                                                UmiStatus *status);
 *     gboolean umi_project_open(UmiProjectManager *pm,
 *                               const char *folder,
 *                               GError **err);
 *     void     umi_project_refresh_index(UmiProjectManager *pm);
 *     void     umi_project_manager_free(UmiProjectManager *pm);
 *
 * - We intentionally do **not** call umi_status_set(...) or any status functions.
 * - We don’t assume any concrete API for UmiWorkspace/UmiFileIndex/UmiRecent:
 *   this keeps compilation robust even if those subsystems are still evolving.
 * - On open(), we validate the folder exists (directory) and return TRUE; you
 *   can later extend this to wire the workspace root or (re)index the tree.
 * - Error reporting uses GError** (if provided) for open() failures only.
 * - Everything is pure C (C17) + GLib; no GtkBuilder, no XML, no GResource.
 *---------------------------------------------------------------------------*/

#include <glib.h>                 /* gboolean, gchar, GError, g_set_error, etc.  */
#include <string.h>               /* (not strictly needed; common safe include)  */

#include "project_manager.h"      /* Must match this header’s struct + prototypes */

/*-----------------------------------------------------------------------------
 * Internal helpers
 *---------------------------------------------------------------------------*/

/* Small, static error domain so callers can distinguish our errors if desired. */
static GQuark
umi_project_error_quark(void)
{
    /* Using a fixed string is fine; this becomes interned as a GQuark.        */
    return g_quark_from_static_string("uside-project-error");
}

/* Set a GError with a formatted message iff 'err' is non-NULL.                */
static void
umi_project_set_error(GError **err, int code, const char *fmt, ...)
{
    if (!err) {
        return; /* Caller not interested in detailed errors.                   */
    }
    va_list ap;
    va_start(ap, fmt);
    gchar *msg = g_strdup_vprintf(fmt, ap);               /* build message     */
    va_end(ap);

    g_set_error(err, umi_project_error_quark(), code, "%s", msg ? msg : "error");
    g_free(msg);
}

/* Validate that a path exists and is a directory.                             */
static gboolean
umi_project_validate_dir(const char *folder, GError **err)
{
    if (G_UNLIKELY(folder == NULL || *folder == '\0')) {
        umi_project_set_error(err, 1, "No folder path provided");
        return FALSE;
    }
    if (!g_file_test(folder, G_FILE_TEST_IS_DIR)) {
        umi_project_set_error(err, 2, "Folder is not a directory or not accessible: %s", folder);
        return FALSE;
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Constructor
 *---------------------------------------------------------------------------*/
UmiProjectManager *
umi_project_manager_new(UmiWorkspace *ws, UmiRecent *recent, UmiStatus *status)
{
    /* Allocate the manager; fields not explicitly set are zero-initialised.    */
    UmiProjectManager *pm = g_new0(UmiProjectManager, 1);

    /* Wire provided subsystems (we do not own them; caller manages lifetime).  */
    pm->ws     = ws;       /* may be NULL if not initialised yet               */
    pm->recent = recent;   /* may be NULL                                      */
    pm->status = status;   /* may be NULL; we won’t call any status API        */

    /* Index is lazily (re)built; keep NULL until refresh/open decides.         */
    pm->index  = NULL;

    /* Informative log without requiring a logging subsystem.                   */
    g_message("UmiProjectManager: created (ws=%p, recent=%p, status=%p)", (void*)ws, (void*)recent, (void*)status);
    return pm;
}

/*-----------------------------------------------------------------------------
 * Open a folder as the workspace root
 *---------------------------------------------------------------------------*/
gboolean
umi_project_open(UmiProjectManager *pm, const char *folder, GError **err)
{
    /* Basic validation of the manager pointer.                                 */
    g_return_val_if_fail(pm != NULL, FALSE);

    /* Validate directory path and existence; set GError on failure.            */
    if (!umi_project_validate_dir(folder, err)) {
        return FALSE;
    }

    /* At this stage, we do not assume any specific Workspace API.
     * Many codebases do something like: umi_workspace_set_root(pm->ws, folder)
     * or they rebuild the file index right away.
     * To stay robust and compile-proof, we only log the action here.
     *
     * If/when the Workspace exposes a clear setter, wire it here.              */
    g_message("UmiProjectManager: open → '%s'", folder);

    /* Success. Caller may trigger a refresh_index() next.                      */
    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Refresh the file index (no-op placeholder)
 *
 * RATIONALE:
 *   The header documents that 'index' is owned here, but we avoid guessing
 *   the concrete API of UmiFileIndex. Later, you can fill this with the real
 *   discovery: e.g., umi_file_index_build(folder), etc., and free old index.
 *---------------------------------------------------------------------------*/
void
umi_project_refresh_index(UmiProjectManager *pm)
{
    g_return_if_fail(pm != NULL);

    /* No concrete UmiFileIndex API is assumed here.                            */
    /* You can implement (re)indexing in a follow-up patch:                     */
    /*   - if (pm->index) umi_file_index_free(pm->index);                       */
    /*   - pm->index = umi_file_index_build( umi_workspace_get_root(pm->ws) );  */

    g_message("UmiProjectManager: refresh_index (placeholder; not implemented yet)");
}

/*-----------------------------------------------------------------------------
 * Destructor
 *---------------------------------------------------------------------------*/
void
umi_project_manager_free(UmiProjectManager *pm)
{
    if (pm == NULL) {
        return; /* tolerate NULL for convenience                                */
    }

    /* If we had created an index instance, we would free it here. We won’t
     * guess the correct free function name to keep this file build-stable.
     * Leaving it NULL preserves safety; you can fill this in later.            */
    pm->index = NULL;

    g_message("UmiProjectManager: destroyed");
    g_free(pm);
}
/*--- end of file ---*/
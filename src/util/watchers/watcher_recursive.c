/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_recursive.c
 *
 * PURPOSE:
 *   Recursive watcher built on GFileMonitor (one monitor per directory).
 *   When any file in the tree changes, we invoke the user callback with a
 *   best-effort UTF-8 path. Pure C; no UI/cross-module headers.
 *
 * DESIGN:
 *   - Track monitors (GFileMonitor*) and root directories we manage.
 *   - Convert file paths to UTF-8 using GLib; avoid following symlink loops.
 *   - WATCH_MOVES to classify renames on backends that support it.
 *
 * SECURITY/ROBUSTNESS:
 *   - Skip symlinked directories to avoid cycles.
 *   - Defensive guards; avoid unbounded prints; strict ownership.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "watcher_recursive.h"

#include <string.h>
#include <errno.h>
#include <glib.h>

/* Private structure */
struct _UmiWatcherRec {
    GPtrArray  *monitors;  /* Array<GFileMonitor*>; owned here                */
    GPtrArray  *roots;     /* Array<char*> of directory roots we manage       */
    UmiWatchCb  cb;        /* User callback                                   */
    gpointer    user;      /* Opaque pointer passed back                      */
};

/* Normalize to a displayable UTF-8 path or URI. */
static char *normalize_path(GFile *f)
{
    if (!f) return g_strdup("(unknown)");
    char *p = g_file_get_path(f);
    if (!p) return g_file_get_uri(f);
#ifdef G_OS_WIN32
    for (char *q = p; *q; ++q) if (*q == '\\') *q = '/';
#endif
    return p;
}

static void mon_changed(GFileMonitor *mon,
                        GFile         *file,
                        GFile         *other_file,
                        GFileMonitorEvent evt,
                        gpointer       u)
{
    (void)mon; (void)other_file; (void)evt;
    UmiWatcherRec *w = (UmiWatcherRec*)u;
    if (!w || !w->cb) return;

    g_autofree char *path = normalize_path(file);
    w->cb(w->user, path ? path : "(unknown)");
}

/* Attach a monitor to a directory with WATCH_MOVES if available. */
static gboolean add_dir_monitor(UmiWatcherRec *w, const char *dir_path)
{
    if (!w || !dir_path || !*dir_path) return FALSE;

    g_autoptr(GFile) dir = g_file_new_for_path(dir_path);
    GError *err = NULL;

    GFileMonitor *m = g_file_monitor_directory(
        dir, G_FILE_MONITOR_WATCH_MOVES, NULL, &err);

    if (!m) {
        g_warning("watcher: monitor failed for '%s': %s",
                  dir_path, err ? err->message : "unknown");
        g_clear_error(&err);
        return FALSE;
    }

    g_signal_connect(m, "changed", G_CALLBACK(mon_changed), w);
    g_ptr_array_add(w->monitors, m); /* owned by array free func */
    return TRUE;
}

/* Depth-first scan with symlink-guard. */
static void scan_dir(UmiWatcherRec *w, const char *root)
{
    if (!w || !root) return;

    add_dir_monitor(w, root);

    GError *err = NULL;
    g_autoptr(GFile) groot = g_file_new_for_path(root);
    g_autoptr(GFileEnumerator) en =
        g_file_enumerate_children(groot,
            G_FILE_ATTRIBUTE_STANDARD_NAME ","
            G_FILE_ATTRIBUTE_STANDARD_TYPE ","
            G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK,
            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
            NULL, &err);
    if (!en) { g_clear_error(&err); return; }

    for (;;) {
        GFileInfo *info = g_file_enumerator_next_file(en, NULL, &err);
        if (!info) break;

        const gboolean is_dir  = (g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY);
        const gboolean is_link = g_file_info_get_is_symlink(info);

        if (is_dir && !is_link) {
            const char *name = g_file_info_get_name(info);
            g_autoptr(GFile) child = g_file_get_child(groot, name);
            g_autofree char *child_path = g_file_get_path(child);
            if (child_path) scan_dir(w, child_path);
        }
        g_object_unref(info);
    }
    g_clear_error(&err);
}

/* If given a file, return its parent directory; else duplicate dir path. */
static char *dir_for_any_path(const char *path)
{
    if (!path || !*path) return NULL;
    if (g_file_test(path, G_FILE_TEST_IS_DIR)) return g_strdup(path);
    g_autofree char *parent = g_path_get_dirname(path);
    return g_strdup(parent);
}

/* Drop all monitors (disconnect + free). */
static void clear_monitors(UmiWatcherRec *w)
{
    if (!w || !w->monitors) return;
    for (guint i = 0; i < w->monitors->len; ++i) {
        GFileMonitor *m = g_ptr_array_index(w->monitors, i);
        if (!m) continue;
        g_signal_handlers_disconnect_by_data(m, w);
        /* array free func will unref in unref below */
    }
    g_ptr_array_set_size(w->monitors, 0);
}

UmiWatcherRec *umi_watchrec_new(const char *root, UmiWatchCb cb, gpointer user)
{
    if (!root || !*root || !cb) return NULL;

    UmiWatcherRec *w = g_new0(UmiWatcherRec, 1);
    w->monitors = g_ptr_array_new_with_free_func((GDestroyNotify)g_object_unref);
    w->roots    = g_ptr_array_new_with_free_func(g_free);
    w->cb       = cb;
    w->user     = user;

    g_ptr_array_add(w->roots, g_strdup(root));
    scan_dir(w, root);
    return w;
}

gboolean umi_watchrec_add(UmiWatcherRec *w, const char *path_or_dir)
{
    if (!w || !path_or_dir || !*path_or_dir) return FALSE;

    g_autofree char *dir = dir_for_any_path(path_or_dir);
    if (!dir) return FALSE;

    for (guint i = 0; i < w->roots->len; ++i) {
        const char *existing = g_ptr_array_index(w->roots, i);
        if (g_strcmp0(existing, dir) == 0) { scan_dir(w, dir); return TRUE; }
    }

    g_ptr_array_add(w->roots, g_strdup(dir));
    scan_dir(w, dir);
    return TRUE;
}

void umi_watchrec_rescan(UmiWatcherRec *w)
{
    if (!w) return;
    clear_monitors(w);
    for (guint i = 0; i < w->roots->len; ++i) {
        const char *r = g_ptr_array_index(w->roots, i);
        scan_dir(w, r);
    }
}

void umi_watchrec_free(UmiWatcherRec *w)
{
    if (!w) return;
    clear_monitors(w);
    g_clear_pointer(&w->monitors, g_ptr_array_unref);
    g_clear_pointer(&w->roots,    g_ptr_array_unref);
    g_free(w);
}

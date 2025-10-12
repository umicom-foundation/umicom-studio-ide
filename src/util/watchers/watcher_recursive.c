/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_recursive.c
 * PURPOSE:
 *   Implementation of a recursive watcher using GFileMonitor per directory.
 *
 * DESIGN:
 *   - Maintain a dynamic array of GFileMonitor* (one per directory).
 *   - “Add” accepts either a directory or a file path; file paths are turned
 *     into their parent directory for monitoring.
 *   - On any change event, we build a best-effort UTF-8 path and invoke the
 *     user callback.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "watcher_recursive.h"

#include <string.h>   /* strcmp */
#include <errno.h>    /* errno */
#include <glib/gprintf.h>

/*-----------------------------------------------------------------------------
 * Private structure
 *---------------------------------------------------------------------------*/
struct _UmiWatcherRec {
    GPtrArray  *monitors;  /* Array<GFileMonitor*>; each element is owned here. */
    GPtrArray  *roots;     /* Array<char*> of directory roots we manage.        */
    UmiWatchCb  cb;        /* User callback to receive event paths.             */
    gpointer    user;      /* Opaque pointer passed back to the callback.       */
};

/*-----------------------------------------------------------------------------
 * Helpers
 *---------------------------------------------------------------------------*/

/* Normalize a path: ensure UTF-8, prefer forward slashes (GLib APIs are fine
 * either way), and avoid NULLs. Return a freshly-allocated string (g_free). */
static char *normalize_path(GFile *f)
{
    if (!f) return g_strdup("(unknown)");
    char *p = g_file_get_path(f);     /* may be NULL for non-native URIs */
    if (!p) return g_file_get_uri(f); /* fallback to URI if path is NULL  */
    /* Optionally, transform backslashes to slashes on Windows for UI consistency. */
#ifdef G_OS_WIN32
    for (char *q = p; *q; ++q) if (*q == '\\') *q = '/';
#endif
    return p; /* caller takes ownership */
}

/* Event handler for a single directory monitor. */
static void mon_changed(GFileMonitor *mon,
                        GFile         *file,
                        GFile         *other_file,
                        GFileMonitorEvent evt,
                        gpointer       u)
{
    (void)mon; (void)other_file; (void)evt;

    UmiWatcherRec *w = (UmiWatcherRec*)u;
    if (!w || !w->cb) return;

    char *path = normalize_path(file);
    /* Invoke user callback (read-only string). */
    w->cb(w->user, path ? path : "(unknown)");
    g_free(path);
}

/* Create and register a monitor for a given directory. */
static gboolean add_dir_monitor(UmiWatcherRec *w, const char *dir_path)
{
    if (!w || !dir_path || !*dir_path) return FALSE;

    g_autoptr(GFile) dir = g_file_new_for_path(dir_path);
    GError *err = NULL;

    /* NOTE: G_FILE_MONITOR_NONE is non-recursive; we implement recursion
     * by attaching a monitor to every subdirectory. */
    GFileMonitor *m = g_file_monitor_directory(dir, G_FILE_MONITOR_NONE, NULL, &err);
    if (!m) {
        g_warning("watcher: failed to monitor '%s': %s", dir_path, err ? err->message : "unknown");
        g_clear_error(&err);
        return FALSE;
    }

    g_signal_connect(m, "changed", G_CALLBACK(mon_changed), w);
    g_ptr_array_add(w->monitors, m); /* monitor owned by array (freed in dispose) */
    return TRUE;
}

/* Depth-first scan: walk 'root' and attach monitors to all directories. */
static void scan_dir(UmiWatcherRec *w, const char *root)
{
    if (!w || !root) return;

    /* Ensure the root itself is monitored. */
    add_dir_monitor(w, root);

    GError *err = NULL;
    g_autoptr(GFile) groot = g_file_new_for_path(root);
    g_autoptr(GFileEnumerator) en =
        g_file_enumerate_children(groot,
                                  G_FILE_ATTRIBUTE_STANDARD_NAME ","
                                  G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                  G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                  NULL, &err);
    if (!en) {
        g_clear_error(&err);
        return;
    }

    GFileInfo *info = NULL;
    while ((info = g_file_enumerator_next_file(en, NULL, &err)) != NULL) {
        GFileType type = g_file_info_get_file_type(info);
        if (type == G_FILE_TYPE_DIRECTORY) {
            const char *name = g_file_info_get_name(info);
            g_autoptr(GFile) child = g_file_get_child(groot, name);
            g_autofree char *child_path = g_file_get_path(child);
            if (child_path) {
                /* Recurse into subdirectory. */
                scan_dir(w, child_path);
            }
        }
        g_object_unref(info);
    }
    g_clear_error(&err);
}

/* Turn an input into a directory path: if given a file, return its dirname. */
static char *dir_for_any_path(const char *path)
{
    if (!path || !*path) return NULL;
    if (g_file_test(path, G_FILE_TEST_IS_DIR))
        return g_strdup(path);

    /* Parent directory of file. */
    g_autofree char *dir = g_path_get_dirname(path);
    return g_strdup(dir);
}

/* Drop all monitors. */
static void clear_monitors(UmiWatcherRec *w)
{
    if (!w || !w->monitors) return;
    for (guint i = 0; i < w->monitors->len; ++i) {
        GFileMonitor *m = g_ptr_array_index(w->monitors, i);
        if (m) g_object_unref(m);
    }
    g_ptr_array_set_size(w->monitors, 0);
}

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/
UmiWatcherRec *umi_watchrec_new(const char *root, UmiWatchCb cb, gpointer user)
{
    if (!root || !*root || !cb) return NULL;

    UmiWatcherRec *w = g_new0(UmiWatcherRec, 1);
    w->monitors = g_ptr_array_new_with_free_func((GDestroyNotify)g_object_unref);
    w->roots    = g_ptr_array_new_with_free_func(g_free);
    w->cb       = cb;
    w->user     = user;

    /* Record initial root and scan. */
    g_ptr_array_add(w->roots, g_strdup(root));
    scan_dir(w, root);

    return w;
}

gboolean umi_watchrec_add(UmiWatcherRec *w, const char *path_or_dir)
{
    if (!w || !path_or_dir || !*path_or_dir) return FALSE;

    g_autofree char *dir = dir_for_any_path(path_or_dir);
    if (!dir) return FALSE;

    /* Avoid duplicate root entries. */
    for (guint i = 0; i < w->roots->len; ++i) {
        const char *existing = g_ptr_array_index(w->roots, i);
        if (g_strcmp0(existing, dir) == 0) {
            /* Already tracked; ensure monitor exists by rescanning just in case. */
            scan_dir(w, dir);
            return TRUE;
        }
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
/*--- end of file ---*/
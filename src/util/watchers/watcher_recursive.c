/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/watcher_recursive.c
 * PURPOSE: Recursively monitor a directory tree using GFileMonitor. Creates a
 *          monitor per directory (children included) and forwards interesting
 *          events to a single user callback.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/watcher_recursive.h"  /* UmiWatchRec, UmiWatchCb */
#include <gio/gio.h>                    /* GFile, GFileMonitor, GFileMonitorEvent */
#include <glib.h>                       /* GLib utilities */
#include <string.h>

/*-----------------------------------------------------------------------------
 * Internal item describing one directory monitor we created during the scan.
 * We keep both the GFileMonitor* (to disconnect/unref on teardown) and the
 * directory path (for diagnostics, potential future filtering, etc.).
 *---------------------------------------------------------------------------*/
typedef struct {
  gchar         *path;  /* Owning copy of directory path we monitor.           */
  GFileMonitor  *mon;   /* Directory monitor for the path above.               */
} Item;

/*-----------------------------------------------------------------------------
 * UmiWatchRec
 * PURPOSE: Opaque handle returned to callers; keeps the root path, the set of
 *          directory monitors, and the callback + user data we forward events to.
 *---------------------------------------------------------------------------*/
struct _UmiWatchRec {
  gchar      *root;   /* Owning copy of the root directory we watch.          */
  GPtrArray  *items;  /* Array<Item*>; each item owns its path and monitor.   */
  UmiWatchCb  cb;     /* User callback to receive event paths.                */
  gpointer    user;   /* User data forwarded unchanged to cb.                 */
};

/*-----------------------------------------------------------------------------
 * mon_changed
 * PURPOSE: GFileMonitor "changed" signal handler. We normalize and forward
 *          only the interesting events (created, deleted, changed, moved).
 *
 * PARAMETERS:
 *   mon        - The monitor that fired (unused for now)
 *   file       - The file/directory whose state changed
 *   other_file - For move events, the "other end" (unused here)
 *   event      - GFileMonitorEvent describing what happened
 *   u          - User data we attached to the signal (UmiWatchRec*)
 *---------------------------------------------------------------------------*/
static void mon_changed(GFileMonitor *mon,
                        GFile *file,
                        GFile *other_file,
                        GFileMonitorEvent event,
                        gpointer u)
{
  (void)mon;         /* We don't need which monitor fired.                   */
  (void)other_file;  /* We don't currently use the paired path for moves.    */

  UmiWatchRec *w = (UmiWatchRec *)u;
  if (!w || !w->cb) {
    /* Nothing to forward to. */
    return;
  }

  /* Convert the GFile into a UTF-8 absolute path. May return NULL if GIO
   * cannot determine a local path for the backend, so guard accordingly. */
  char *path = g_file_get_path(file);

  /* Forward only a subset of events to keep noise down:
   * - CHANGED: file contents or attributes changed
   * - CREATED: a new child entry appeared
   * - DELETED: an entry disappeared
   * - MOVED:   entry was moved/renamed (paired with other_file) */
  if (event == G_FILE_MONITOR_EVENT_CHANGED ||
      event == G_FILE_MONITOR_EVENT_CREATED ||
      event == G_FILE_MONITOR_EVENT_DELETED ||
      event == G_FILE_MONITOR_EVENT_MOVED)
  {
    w->cb(w->user, path ? path : "(unknown)"); /* Caller treats string as read-only. */
  }

  g_free(path);  /* Always free what g_file_get_path() returned. */
}

/*-----------------------------------------------------------------------------
 * add_dir_monitor
 * PURPOSE: Create a GFileMonitor for a directory path and connect its "changed"
 *          signal. On success, append an Item* to w->items (ownership kept by w).
 * RETURNS:
 *   gboolean TRUE on success; FALSE on failure (monitor not created).
 *---------------------------------------------------------------------------*/
static gboolean add_dir_monitor(UmiWatchRec *w, const char *dir_path)
{
  if (!w || !dir_path) return FALSE;

  /* Wrap the path in a GFile object. */
  GFile *gf = g_file_new_for_path(dir_path);

  /* Choose monitor flags:
   * - WATCH_MOVES: get MOVED/MOVED_IN/MOVED_OUT if backend supports it
   * - NONE otherwise; we rely on default semantics for CHANGED/CREATED/DELETED */
  const GFileMonitorFlags flags = G_FILE_MONITOR_WATCH_MOVES;

  /* Create a directory monitor; NULL GError* as we can tolerate failure here. */
  GError *err = NULL;
  GFileMonitor *mon = g_file_monitor_directory(gf, flags, NULL, &err);

  if (!mon) {
    /* Monitor creation failed (permissions/back-end). Clean up and continue. */
    if (err) g_error_free(err);
    g_object_unref(gf);
    return FALSE;
  }

  /* Allocate and populate our bookkeeping item. */
  Item *it = g_new0(Item, 1);
  it->path = g_strdup(dir_path);  /* Own a copy for diagnostics/future use. */
  it->mon  = mon;                 /* We own + will unref in tear-down.     */

  /* Connect signal; 'w' is the user data we get back in mon_changed(). */
  g_signal_connect(mon, "changed", G_CALLBACK(mon_changed), w);

  /* Track it; array owns the pointer value, but not the Item fields. */
  g_ptr_array_add(w->items, it);

  g_object_unref(gf); /* GFile served its purpose; unref now. */
  return TRUE;
}

/*-----------------------------------------------------------------------------
 * scan_dir
 * PURPOSE: Recursively walk a directory tree, creating a monitor for each
 *          directory and recursing into subdirectories (skipping dot-entries).
 *---------------------------------------------------------------------------*/
static void scan_dir(UmiWatchRec *w, const char *root)
{
  if (!w || !root) return;

  /* First, create a monitor for the current directory. If it fails, we still
   * attempt to recurse—though without the parent monitor we’ll see fewer events. */
  add_dir_monitor(w, root);

  /* Open the directory for iteration. We ignore errors here to keep the
   * watcher resilient (e.g., permission denied). */
  GError *err = NULL;
  GDir *d = g_dir_open(root, 0, &err);
  if (!d) {
    if (err) g_error_free(err);
    return;
  }

  const gchar *name = NULL;
  while ((name = g_dir_read_name(d))) {
    /* Skip hidden/dot entries to reduce noise and avoid infinite-ish loops. */
    if (name[0] == '.') continue;

    /* Join to get the full path; check if this entry is a directory. */
    gchar *path = g_build_filename(root, name, NULL);
    if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
      /* Recurse into subdirectory: we create a monitor there as well. */
      scan_dir(w, path);
    }
    g_free(path);
  }

  g_dir_close(d);
}

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

/* Create a new recursive watcher rooted at 'root'. Returns NULL on failure. */
UmiWatchRec *umi_watchrec_new(const char *root, UmiWatchCb cb, gpointer user)
{
  if (!root || !*root || !cb) {
    /* Require a non-empty root and a valid callback. */
    return NULL;
  }

  UmiWatchRec *w = g_new0(UmiWatchRec, 1);
  w->root  = g_strdup(root);            /* Own copy of the root path.  */
  w->items = g_ptr_array_new();         /* Will hold Item* elements.   */
  w->cb    = cb;                        /* Store callback.             */
  w->user  = user;                      /* Store user data.            */

  /* Build monitors for root and all subdirectories (depth-first). */
  scan_dir(w, root);
  return w;
}

/* Tear down all monitors and free the watcher object. Safe with NULL. */
void umi_watchrec_free(UmiWatchRec *w)
{
  if (!w) return;

  /* Disconnect and free all per-directory monitors we created. */
  for (guint i = 0; i < w->items->len; i++) {
    Item *it = (Item *)g_ptr_array_index(w->items, i);
    if (!it) continue;

    if (it->mon) {
      /* Disconnect any handlers tied to 'w' and release the monitor. */
      g_signal_handlers_disconnect_by_data(it->mon, w);
      g_object_unref(it->mon);
      it->mon = NULL;
    }

    g_clear_pointer(&it->path, g_free);
    g_free(it);
  }

  /* Clear the pointer array storage itself. */
  g_ptr_array_free(w->items, TRUE);
  w->items = NULL;

  /* Free the root path and the container. */
  g_clear_pointer(&w->root, g_free);
  g_free(w);
}

/* Rescan the directory tree: drop existing monitors, then rebuild from root. */
void umi_watchrec_rescan(UmiWatchRec *w)
{
  if (!w) return;

  /* Remove/cleanup all existing monitors. */
  for (guint i = 0; i < w->items->len; i++) {
    Item *it = (Item *)g_ptr_array_index(w->items, i);
    if (!it) continue;
    if (it->mon) {
      g_signal_handlers_disconnect_by_data(it->mon, w);
      g_object_unref(it->mon);
      it->mon = NULL;
    }
    g_clear_pointer(&it->path, g_free);
    g_free(it);
  }
  g_ptr_array_set_size(w->items, 0);

  /* Rebuild a fresh set rooted at the original w->root. */
  scan_dir(w, w->root);
}

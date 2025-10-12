/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/path_watcher.c
 * PURPOSE: Thin wrapper around GFileMonitor for simple, non-recursive watches.
 *          Useful for ad-hoc paths where a full recursive watcher is overkill.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "path_watcher.h"  /* UmiPathWatcher, UmiPathEvt */
#include <gio/gio.h>               /* GFile, GFileMonitor */
#include <glib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
 * Internal container: holds any number of GFileMonitor handles (one per path)
 * and a callback target. Not recursive: each monitor is for the exact path.
 *---------------------------------------------------------------------------*/
struct _UmiPathWatcher {
  GPtrArray  *monitors;  /* Array<GFileMonitor*> we own and unref on stop().    */
  GPtrArray  *src_ids;   /* Array<guint> for future GLib source IDs (optional). */
  UmiPathEvt  cb;        /* User callback for events.                            */
  gpointer    user;      /* Forwarded unchanged to cb.                           */
};

/* Forward declaration for the monitor's signal handler. */
static void on_file_changed(GFileMonitor *mon,
                            GFile *file,
                            GFile *other_file,
                            GFileMonitorEvent event,
                            gpointer u);

/*-----------------------------------------------------------------------------
 * umi_pathwatch_new
 * PURPOSE: Allocate a new empty watcher container with a user callback.
 *---------------------------------------------------------------------------*/
UmiPathWatcher *umi_pathwatch_new(UmiPathEvt cb, gpointer user)
{
  UmiPathWatcher *w = g_new0(UmiPathWatcher, 1);
  w->monitors = g_ptr_array_new(); /* No element free func: we unref manually. */
  w->src_ids  = g_ptr_array_new(); /* Currently unused; kept for future timers. */
  w->cb       = cb;
  w->user     = user;
  return w;
}

/*-----------------------------------------------------------------------------
 * umi_pathwatch_add
 * PURPOSE: Start watching a single directory path (non-recursive).
 * NOTES:
 *   - Returns FALSE if the path cannot be monitored (permissions/back-end).
 *   - We request move notifications to receive MOVED/MOVED_IN/MOVED_OUT.
 *---------------------------------------------------------------------------*/
gboolean umi_pathwatch_add(UmiPathWatcher *w, const char *dir_path)
{
  if (!w || !dir_path || !*dir_path) return FALSE;

  /* Create a GFile for the provided path and try to monitor the directory. */
  GFile *gf = g_file_new_for_path(dir_path);

  /* If you need file-level (not directory) monitoring later, switch to
   * g_file_monitor_file(); for now we only monitor directories. */
  const GFileMonitorFlags flags = G_FILE_MONITOR_WATCH_MOVES;

  GError *err = NULL;
  GFileMonitor *mon = g_file_monitor_directory(gf, flags, NULL, &err);
  g_object_unref(gf);  /* GFile no longer needed after monitor creation. */

  if (!mon) {
    if (err) g_error_free(err);
    return FALSE; /* Gracefully report failure without side effects. */
  }

  /* Wire the monitor's "changed" signal to our handler. */
  g_signal_connect(mon, "changed", G_CALLBACK(on_file_changed), w);

  /* Track this monitor so we can disconnect/unref it on stop(). */
  g_ptr_array_add(w->monitors, mon);
  return TRUE;
}

/*-----------------------------------------------------------------------------
 * on_file_changed
 * PURPOSE: Convert GFile to a UTF-8 path and forward the event to the user
 *          callback (if any). We do not filter by event type here; callers can
 *          decide what to do. This keeps the wrapper neutral.
 *---------------------------------------------------------------------------*/
static void on_file_changed(GFileMonitor *mon,
                            GFile *file,
                            GFile *other_file,
                            GFileMonitorEvent event,
                            gpointer u)
{
  (void)mon;         /* Not used. */
  (void)other_file;  /* Not used. */
  (void)event;       /* Not filtered here (wrapper stays generic). */

  UmiPathWatcher *w = (UmiPathWatcher *)u;
  if (!w) return;

  /* Get a best-effort, absolute filesystem path (may be NULL on some backends). */
  char *p = g_file_get_path(file);
  if (w->cb) {
    w->cb(w->user, p ? p : "(unknown)");
  }
  g_free(p);
}

/*-----------------------------------------------------------------------------
 * umi_pathwatch_stop
 * PURPOSE: Stop all active monitors and clear GLib source IDs (if used).
 *---------------------------------------------------------------------------*/
void umi_pathwatch_stop(UmiPathWatcher *w)
{
  if (!w) return;

  /* Disconnect handler(s) bound with 'w' and release each monitor. */
  for (guint i = 0; i < w->monitors->len; i++) {
    GFileMonitor *m = (GFileMonitor *)g_ptr_array_index(w->monitors, i);
    if (!m) continue;
    g_signal_handlers_disconnect_by_data(m, w);
    g_object_unref(m);
  }
  g_ptr_array_set_size(w->monitors, 0);

  /* If we ever register GLib timeouts/idle sources, remove them here. */
  for (guint i = 0; i < w->src_ids->len; i++) {
    guint sid = GPOINTER_TO_UINT(g_ptr_array_index(w->src_ids, i));
    if (sid) g_source_remove(sid);
  }
  g_ptr_array_set_size(w->src_ids, 0);
}

/*-----------------------------------------------------------------------------
 * umi_pathwatch_free
 * PURPOSE: Free the container and its arrays. Call umi_pathwatch_stop() first
 *          if you want to explicitly stop monitoring; free() is tolerant too.
 *---------------------------------------------------------------------------*/
void umi_pathwatch_free(UmiPathWatcher *w)
{
  if (!w) return;

  /* Ensure we don’t leak monitors if caller forgot to stop. */
  umi_pathwatch_stop(w);

  /* Free arrays (TRUE => free the GPtrArray containers). */
  g_ptr_array_free(w->monitors, TRUE);
  g_ptr_array_free(w->src_ids,  TRUE);
  g_free(w);
}

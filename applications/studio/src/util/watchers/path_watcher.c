/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/path_watcher.c
 *
 * PURPOSE:
 *   Thin wrapper around GFileMonitor for simple, NON-RECURSIVE directory
 *   watches. Ideal for ad-hoc paths where a full recursive watcher is overkill.
 *
 * DESIGN:
 *   - Opaque handle holds an array of GFileMonitor*; we own & unref them.
 *   - One monitor per added directory. Event callback is neutral (no filter).
 *   - No cross-module includes; no UI headers.
 *
 * SECURITY/ROBUSTNESS:
 *   - UTF-8 path conversion via GLib; placeholder string on failure.
 *   - All pointers guarded; no unsafe casts or varargs.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "path_watcher.h"  /* public API */
#include <gio/gio.h>
#include <glib.h>

/* Internal container: holds monitors and a user callback. */
struct _UmiPathWatcher {
  GPtrArray  *monitors;  /* Array<GFileMonitor*> owned here                 */
  UmiPathEvt  cb;        /* User callback                                   */
  gpointer    user;      /* Opaque user pointer passed back on events       */
};

/* Forward declaration */
static void on_file_changed(GFileMonitor *mon,
                            GFile *file,
                            GFile *other_file,
                            GFileMonitorEvent event,
                            gpointer u);

UmiPathWatcher *umi_pathwatch_new(UmiPathEvt cb, gpointer user)
{
  UmiPathWatcher *w = g_new0(UmiPathWatcher, 1);
  w->monitors = g_ptr_array_new_with_free_func((GDestroyNotify)g_object_unref);
  w->cb       = cb;
  w->user     = user;
  return w;
}

gboolean umi_pathwatch_add(UmiPathWatcher *w, const char *dir_path)
{
  if (!w || !dir_path || !*dir_path) return FALSE;

  g_autoptr(GFile) gf = g_file_new_for_path(dir_path);
  GError *err = NULL;

  /* WATCH_MOVES gets MOVED[_IN|_OUT] classification on supported backends. */
  GFileMonitor *mon = g_file_monitor_directory(
      gf, G_FILE_MONITOR_WATCH_MOVES, NULL, &err);

  if (!mon) {
    if (err) { g_warning("pathwatch: %s", err->message); g_error_free(err); }
    return FALSE;
  }

  g_signal_connect(mon, "changed", G_CALLBACK(on_file_changed), w);
  g_ptr_array_add(w->monitors, mon);
  return TRUE;
}

static void on_file_changed(GFileMonitor *mon,
                            GFile *file,
                            GFile *other_file,
                            GFileMonitorEvent event,
                            gpointer u)
{
  (void)mon; (void)other_file; (void)event;
  UmiPathWatcher *w = (UmiPathWatcher*)u;
  if (!w) return;

  g_autofree char *p = g_file_get_path(file);
  if (w->cb) w->cb(w->user, p ? p : "(unknown)");
}

void umi_pathwatch_stop(UmiPathWatcher *w)
{
  if (!w) return;
  for (guint i = 0; i < w->monitors->len; ++i) {
    GFileMonitor *m = g_ptr_array_index(w->monitors, i);
    if (!m) continue;
    g_signal_handlers_disconnect_by_data(m, w);
    /* element free func will unref in g_ptr_array_set_size below */
  }
  g_ptr_array_set_size(w->monitors, 0);
}

void umi_pathwatch_free(UmiPathWatcher *w)
{
  if (!w) return;
  umi_pathwatch_stop(w);
  g_ptr_array_free(w->monitors, TRUE);
  g_free(w);
}

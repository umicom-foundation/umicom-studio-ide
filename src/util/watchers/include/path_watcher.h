/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/path_watcher.h
 * PURPOSE:
 *   Public API for a thin, non-recursive path watcher built on GFileMonitor.
 *   This header matches the implementation in path_watcher.c and exposes:
 *     - UmiPathWatcher (opaque handle)
 *     - UmiPathEvt     (callback type: user + path string)
 *     - umi_pathwatch_new/add/stop/free
 *
 * Notes:
 *   - Non-recursive: each added directory is watched at a single level.
 *   - Threading: GLib/GIO signal callbacks run on the main context.
 *   - Lifetime: stop() detaches signals and unreferences monitors; free()
 *               releases the container afterwards.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PATH_WATCHER_H
#define UMICOM_PATH_WATCHER_H

#include <glib.h>   /* gboolean, gpointer */

G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * Compatibility struct used elsewhere (e.g., recursive watcher helpers).
 * Keep as-is so other modules that include this header keep compiling.
 *---------------------------------------------------------------------------*/
typedef struct _UmiPathWatch {
  char     *path;        /* directory path to watch                          */
  gboolean  recursive;   /* whether caller intends recursive monitoring       */
} UmiPathWatch;

/*-----------------------------------------------------------------------------
 * UmiPathEvt: user callback signature
 * @user: user data provided at construction time
 * @path: path string related to the change (never NULL; a placeholder like
 *        "(unknown)" may be passed if GIO didn’t provide one)
 *---------------------------------------------------------------------------*/
typedef void (*UmiPathEvt)(gpointer user, const char *path);

/* Opaque watcher handle (defined privately in path_watcher.c). */
typedef struct _UmiPathWatcher UmiPathWatcher;

/*-----------------------------------------------------------------------------
 * umi_pathwatch_new
 * Create an empty watcher container with a user callback.
 * The watcher owns the list of monitors you add later with add().
 *---------------------------------------------------------------------------*/
UmiPathWatcher *umi_pathwatch_new(UmiPathEvt cb, gpointer user);

/*-----------------------------------------------------------------------------
 * umi_pathwatch_add
 * Start watching a single directory (non-recursive).
 *
 * Returns:
 *   TRUE  -> monitoring started for this directory
 *   FALSE -> could not create a monitor (permissions, backend, invalid path)
 *---------------------------------------------------------------------------*/
gboolean        umi_pathwatch_add(UmiPathWatcher *w, const char *dir_path);

/*-----------------------------------------------------------------------------
 * umi_pathwatch_stop
 * Disconnect signals and unref all underlying monitors.
 * Safe to call multiple times.
 *---------------------------------------------------------------------------*/
void            umi_pathwatch_stop(UmiPathWatcher *w);

/*-----------------------------------------------------------------------------
 * umi_pathwatch_free
 * Free the watcher container itself. Call stop() first if you want to detach
 * monitors explicitly; free() will also ensure resources are released.
 * Safe on NULL.
 *---------------------------------------------------------------------------*/
void            umi_pathwatch_free(UmiPathWatcher *w);

G_END_DECLS

#endif /* UMICOM_PATH_WATCHER_H */
/*--- end of file ---*/

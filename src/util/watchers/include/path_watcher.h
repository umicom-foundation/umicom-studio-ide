/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/path_watcher.h
 *
 * PURPOSE:
 *   Public API for a thin, NON-RECURSIVE path watcher built on GFileMonitor.
 *   Use this when you only need to watch a handful of directories at the
 *   top level. For deep trees, prefer watcher_recursive.h.
 *
 * DESIGN:
 *   - Opaque handle (UmiPathWatcher); construction injects a callback.
 *   - One GFileMonitor per added directory; no cross-module includes.
 *   - GLib/GIO main-context delivery; keep callbacks fast.
 *
 * API:
 *   typedef struct _UmiPathWatch { char *path; gboolean recursive; }  // compat
 *   typedef void (*UmiPathEvt)(gpointer user, const char *path);
 *   UmiPathWatcher *umi_pathwatch_new (UmiPathEvt cb, gpointer user);
 *   gboolean        umi_pathwatch_add (UmiPathWatcher *w, const char *dir_path);
 *   void            umi_pathwatch_stop(UmiPathWatcher *w);
 *   void            umi_pathwatch_free(UmiPathWatcher *w);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PATH_WATCHER_H
#define UMICOM_PATH_WATCHER_H

#include <glib.h>

G_BEGIN_DECLS

/* Kept for compatibility with call sites that pass "recursive" intents. */
typedef struct _UmiPathWatch {
  char     *path;        /* directory path to watch                          */
  gboolean  recursive;   /* ignored by this module (non-recursive by design) */
} UmiPathWatch;

/* User callback signature:
 *  - @user : opaque pointer from constructor
 *  - @path : UTF-8 path related to the change (never NULL; may be "(unknown)")
 */
typedef void (*UmiPathEvt)(gpointer user, const char *path);

/* Opaque watcher handle. */
typedef struct _UmiPathWatcher UmiPathWatcher;

UmiPathWatcher *umi_pathwatch_new(UmiPathEvt cb, gpointer user);
gboolean        umi_pathwatch_add(UmiPathWatcher *w, const char *dir_path);
void            umi_pathwatch_stop(UmiPathWatcher *w);
void            umi_pathwatch_free(UmiPathWatcher *w);

G_END_DECLS
#endif /* UMICOM_PATH_WATCHER_H */

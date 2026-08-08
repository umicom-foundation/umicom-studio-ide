/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_recursive.h
 *
 * PURPOSE:
 *   Public API for a simple, RECURSIVE filesystem watcher built on GIO.
 *   We scan roots, attach a monitor to every subdirectory, and invoke the
 *   user callback with a best-effort UTF-8 path on any change.
 *
 * DESIGN:
 *   - Opaque handle + tiny API: new/add/rescan/free.
 *   - Delivery occurs on GLib’s main context; keep callbacks fast or marshal.
 *   - Cross-platform: GIO selects inotify/FSEvents/ReadDirChangesW/etc.
 *
 * API:
 *   typedef void (*UmiWatchCb)(gpointer user, const char *path);
 *   typedef struct _UmiWatcherRec UmiWatcherRec;
 *   UmiWatcherRec *umi_watchrec_new (const char *root, UmiWatchCb cb, gpointer user);
 *   gboolean       umi_watchrec_add (UmiWatcherRec *w, const char *path_or_dir);
 *   void           umi_watchrec_rescan(UmiWatcherRec *w);
 *   void           umi_watchrec_free  (UmiWatcherRec *w);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H

#include <glib.h>
#include <gio/gio.h>

#include "path_watcher.h" /* UmiPathWatch descriptor for interoperability */

typedef void (*UmiWatchCb)(gpointer user, const char *path);
typedef struct _UmiWatcherRec UmiWatcherRec;

UmiWatcherRec *umi_watchrec_new(const char *root, UmiWatchCb cb, gpointer user);
gboolean       umi_watchrec_add(UmiWatcherRec *w, const char *path_or_dir);
void           umi_watchrec_rescan(UmiWatcherRec *w);
void           umi_watchrec_free(UmiWatcherRec *w);

#endif /* UMICOM_WATCHER_RECURSIVE_H */

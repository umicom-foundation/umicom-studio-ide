/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_recursive.h
 * PURPOSE:
 *   Public API for a simple, recursive file-system watcher built on GIO.
 *   It scans a root directory, attaches a monitor to every subdirectory,
 *   and invokes a user-supplied callback with the changed path.
 *
 * DESIGN:
 *   - Keep the interface tiny: opaque handle + start/add/rescan/free.
 *   - Callbacks are synchronous on GLib’s default main context; caller
 *     must be on (or marshal into) the UI thread if touching GTK.
 *   - We deliberately avoid platform-specific backends; GIO picks the
 *     best mechanism (inotify/FSEvents/ReadDirectoryChangesW/etc.).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H

#include <glib.h>     /* gpointer, gboolean */
#include <gio/gio.h>  /* GFileMonitor, GFileMonitorEvent */

#include "path_watcher.h" /* UmiPathWatch request descriptor */

/*-----------------------------------------------------------------------------
 * Typedefs
 *---------------------------------------------------------------------------*/

/* User callback signature:
 *  - user : user pointer passed during construction
 *  - path : normalized UTF-8 path that changed (read-only, do not free)
 */
typedef void (*UmiWatchCb)(gpointer user, const char *path);

/* Opaque type managed by this module. */
typedef struct _UmiWatcherRec UmiWatcherRec;

/*-----------------------------------------------------------------------------
 * API
 *---------------------------------------------------------------------------*/

/* Create a recursive watcher rooted at 'root'. Immediately scans and attaches
 * monitors to all directories under 'root'. 'cb' is called for relevant file
 * events; it MUST be fast or dispatch to another thread to avoid blocking the
 * main loop. Ownership: the returned pointer must be freed with
 * umi_watchrec_free(). Returns NULL on allocation/monitor failure. */
UmiWatcherRec *umi_watchrec_new(const char *root, UmiWatchCb cb, gpointer user);

/* Add an additional directory (or file’s parent dir) to the recursive watch.
 * If a file path is provided, the parent directory is monitored and events are
 * filtered to that file by the caller if desired. Returns TRUE on success. */
gboolean       umi_watchrec_add(UmiWatcherRec *w, const char *path_or_dir);

/* Re-scan the current roots, drop old monitors, and re-attach to reflect the
 * current on-disk tree layout. Safe to call after bulk changes. */
void           umi_watchrec_rescan(UmiWatcherRec *w);

/* Stop all monitoring and free resources. */
void           umi_watchrec_free(UmiWatcherRec *w);

#endif /* UMICOM_WATCHER_RECURSIVE_H */

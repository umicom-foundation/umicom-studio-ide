/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_recursive.h
 * PURPOSE: Recursive filesystem watcher API
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/*=============================================================================
 *  watcher_recursive.h  —  Recursive filesystem watch utilities (GLib/GIO)
 *
 *  NOTE (added): This header fixes an unterminated include guard that caused:
 *    "error: unterminated #ifndef" at line 8 during compilation.
 *  We keep the original naming scheme and add a small request struct
 *  (UmiPathWatch) that other modules (watcher_integration) already reference.
 *
 *  This module provides:
 *    - Opaque handle type:       UmiWatcherRec
 *    - Event callback type:      UmiWatcherCallback
 *    - Back-compat typedefs:     UmiWatchRec, UmiWatchCb
 *    - Minimal request struct:   UmiPathWatch  (root + recursive flag)
 *    - Public API:
 *        UmiWatcherRec *umi_watchrec_new(const char *root,
 *                                        UmiWatcherCallback cb,
 *                                        gpointer user);
 *        void           umi_watchrec_free(UmiWatcherRec *w);
 *        void           umi_watchrec_rescan(UmiWatcherRec *w);
 *
 *============================================================================*/

#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H

/* Keep pragma once too; redundant but harmless. */
#pragma once

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 *  Opaque handle for the recursive watcher implementation.
 *---------------------------------------------------------------------------*/
typedef struct _UmiWatcherRec UmiWatcherRec;

/*-----------------------------------------------------------------------------
 *  Callback invoked for each filesystem event.
 *
 *  @param user  Opaque pointer supplied at construction time.
 *  @param path  UTF-8 path for the file/dir involved in the event (never NULL;
 *               when unavailable we pass "(unknown)").
 *---------------------------------------------------------------------------*/
typedef void (*UmiWatcherCallback)(gpointer user, const char *path);

/* Backwards compatibility with earlier names seen in previous source:
 *  - UmiWatchRec  (old name for UmiWatcherRec)
 *  - UmiWatchCb   (old name for UmiWatcherCallback)
 */
typedef UmiWatcherRec      UmiWatchRec;
typedef UmiWatcherCallback UmiWatchCb;

/*-----------------------------------------------------------------------------
 *  UmiPathWatch — request description used by integration layer.
 *
 *  Some compilation units (e.g., watcher_integration) reference this
 *  structure, so define it here to centralize types and avoid cross-header
 *  cycles. Extend as needed (e.g., include/exclude patterns).
 *---------------------------------------------------------------------------*/
typedef struct
{
  const char *root;     /* Directory root to watch (UTF-8, absolute or relative). */
  gboolean    recursive;/* If TRUE, watch subdirectories recursively.             */
} UmiPathWatch;

/*-----------------------------------------------------------------------------
 *  Create a new recursive watcher.
 *
 *  The watcher will immediately install monitors for @root and all existing
 *  subdirectories. Newly created subdirectories are detected and monitored
 *  automatically. The callback is invoked on changes.
 *
 *  Returns: non-NULL on success; NULL on fatal setup error.
 *---------------------------------------------------------------------------*/
UmiWatcherRec *umi_watchrec_new(const char *root,
                                UmiWatcherCallback cb,
                                gpointer user);

/*-----------------------------------------------------------------------------
 *  Free a watcher and release all associated monitors/resources.
 *---------------------------------------------------------------------------*/
void umi_watchrec_free(UmiWatcherRec *w);

/*-----------------------------------------------------------------------------
 *  Re-scan the directory tree:
 *   - Uninstalls all existing directory monitors
 *   - Rebuilds the monitor set from the current on-disk state
 *---------------------------------------------------------------------------*/
void umi_watchrec_rescan(UmiWatcherRec *w);

G_END_DECLS

#endif /* UMICOM_WATCHER_RECURSIVE_H */
/*--- end of file ---*/
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_recursive.h
 * PURPOSE: Recursive filesystem watcher API
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H
#pragma once
/*
 * Recursive filesystem watcher (GLib/GIO)
 *
 * Canonical API:
 *   - UmiWatcherCallback
 *   - UmiWatcherRec
 *   - umi_watchrec_new()
 *   - umi_watchrec_free()
 *   - umi_watchrec_rescan()
 *
 * Back-compat aliases are provided for older names used in the tree:
 *   - UmiWatchCb      -> UmiWatcherCallback
 *   - UmiWatchRec     -> UmiWatcherRec
 */

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* Opaque watcher handle */
typedef struct UmiWatcherRec UmiWatcherRec;

/* Callback invoked on any file change event under the watched root.
 * The 'path' string is UTF‑8 and valid for the duration of the call.
 * Treat it as read-only. */
typedef void (*UmiWatcherCallback)(gpointer user, const char *path);

/* Backward-compat typedefs (older names that appear in some sources) */
typedef UmiWatcherCallback UmiWatchCb;
typedef UmiWatcherRec      UmiWatchRec;

/* Create a recursive watcher rooted at 'root'.
 * - 'root' must be an absolute or relative directory path.
 * - 'cb'   will be called on changes (create/delete/modify/move).
 * - 'user' is passed back to 'cb' on every invocation.
 * Returns NULL on error.
 */
UmiWatcherRec *umi_watchrec_new(const char *root, UmiWatcherCallback cb, gpointer user);

/* Free the watcher and stop all monitors (idempotent). */
void umi_watchrec_free(UmiWatcherRec *w);

/* Drop all monitors and rescan the tree (useful after large moves). */
void umi_watchrec_rescan(UmiWatcherRec *w);

G_END_DECLS

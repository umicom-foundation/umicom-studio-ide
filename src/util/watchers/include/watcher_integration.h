/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_integration.h
 * PURPOSE:
 *   Glue between the recursive watcher and our UI/workspace components:
 *   when filesystem events arrive, refresh the FileTree (and optionally
 *   inform the WorkspaceState).
 *
 * DESIGN:
 *   - Keep this as a very thin layer: own a UmiWatcherRec and marshal its
 *     callback into the UI refresh points only.
 *   - The caller adds one or more UmiPathWatch requests; the integration
 *     ensures recursion by delegating to the recursive watcher module.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_WATCHER_INTEGRATION_H
#define UMICOM_WATCHER_INTEGRATION_H

#include <glib.h>

#include "watcher_recursive.h" /* UmiWatcherRec, UmiPathWatch, UmiWatchCb */

/* Forward decls to avoid heavy headers here. */
typedef struct _FileTree        FileTree;
typedef struct _WorkspaceState  WorkspaceState;

/* Opaque integration object. */
typedef struct _UmiWatcherIntegration UmiWatcherIntegration;

/* Create the integration. The FileTree reference is borrowed; the caller
 * guarantees its lifetime outlives the integration or calls free() first. */
UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws);

/* Add a watch request (path + recursive flag). Currently we honor the
 * 'recursive' bit always by delegating to the recursive watcher. */
gboolean umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req);

/* Tear down and free all resources. */
void     umi_watch_integ_free(UmiWatcherIntegration *wi);

#endif /* UMICOM_WATCHER_INTEGRATION_H */
/*--- end of file ---*/
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_integration.h
 *
 * PURPOSE:
 *   Glue between the recursive watcher and UI/workspace components.
 *   On any FS event, we refresh the FileTree; workspace state is optional.
 *
 * API:
 *   typedef struct _UmiWatcherIntegration UmiWatcherIntegration;
 *   UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws);
 *   gboolean               umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req);
 *   void                   umi_watch_integ_free(UmiWatcherIntegration *wi);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WATCHER_INTEGRATION_H
#define UMICOM_WATCHER_INTEGRATION_H

#include <glib.h>
#include "watcher_recursive.h"

typedef struct _FileTree       FileTree;
typedef struct _WorkspaceState WorkspaceState;

typedef struct _UmiWatcherIntegration UmiWatcherIntegration;

UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws);
gboolean umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req);
void     umi_watch_integ_free(UmiWatcherIntegration *wi);

#endif /* UMICOM_WATCHER_INTEGRATION_H */

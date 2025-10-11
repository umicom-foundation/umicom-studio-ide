/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_integration.h
 * PURPOSE: Glue between watchers and Workspace/FileTree (signals + callbacks)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_WATCHER_INTEGRATION_H
#define UMICOM_WATCHER_INTEGRATION_H

#include <glib.h>              /* GMainContext, GSource, gboolean */
#include <watcher_recursive.h> /* UmiWatcherRec, UmiPathWatch     */

/* Forward declarations to avoid heavy includes in this public header. */
/* (We only use pointers to these types.)                              */
typedef struct _FileTree        FileTree;
typedef struct _WorkspaceState  WorkspaceState;

/* Bridge object that owns a watcher and updates the project tree + workspace. */
typedef struct {
    UmiWatcherRec *wr;        /* underlying recursive watcher (owned)         */
    FileTree      *tree;      /* project file tree (not owned)                */
    WorkspaceState*ws;        /* workspace state and settings (not owned)     */
    GSource       *source;    /* glib source returned from attach (owned)     */
} UmiWatcherIntegration;

/* Create the integration layer; does not start watching until configured. */
UmiWatcherIntegration *umi_watch_integ_new(FileTree *tree, WorkspaceState *ws);

/* Add a path request (possibly recursive). Returns FALSE on failure. */
gboolean umi_watch_integ_add(UmiWatcherIntegration *wi, const UmiPathWatch *req);

/* Attach to a main context. Returns TRUE on success. */
gboolean umi_watch_integ_attach(UmiWatcherIntegration *wi, GMainContext *ctx);

/* Dispose the integration object and detach from the loop if attached. */
void     umi_watch_integ_free(UmiWatcherIntegration *wi);

#endif /* UMICOM_WATCHER_INTEGRATION_H */

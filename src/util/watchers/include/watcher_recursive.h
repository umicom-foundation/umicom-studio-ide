/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/watcher_recursive.h
 * PURPOSE: Recursive filesystem watcher API
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H

#include <glib.h>  /* GSource, GMainContext, GPtrArray */
#include <path_watcher.h> /* UmiPathWatch */

/* Forward-declared opaque types for the watcher implementation. */
typedef struct _UmiWatcherRec  UmiWatcherRec;

/* Create/destroy a recursive watcher object. Ownership: caller must free. */
UmiWatcherRec *umi_watcher_rec_new(void);        /* allocate empty watcher    */
void            umi_watcher_rec_free(UmiWatcherRec *wr); /* free resources    */

/* Add a watch request to the watcher. Returns FALSE on failure.              */
gboolean umi_watcher_rec_add(UmiWatcherRec *wr, const UmiPathWatch *req);

/* Integrate with GLib main loop: attach to a context and return a GSource*.   */
/* The returned source is owned by the caller (unref when no longer needed).    */
GSource *umi_watcher_rec_attach(UmiWatcherRec *wr, GMainContext *ctx);

#endif /* UMICOM_WATCHER_RECURSIVE_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/path_watcher.h
 * PURPOSE: Shallow path watcher to trigger refresh callbacks
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PATH_WATCHER_H
#define UMICOM_PATH_WATCHER_H

#include <gio/gio.h>

typedef void (*UmiWatchRefreshCb)(gpointer user);

typedef struct _UmiPathWatcher UmiPathWatcher;

UmiPathWatcher *umi_watch_new(const char *root, UmiWatchRefreshCb cb, gpointer user);
void            umi_watch_set_root(UmiPathWatcher *w, const char *root);
void            umi_watch_free(UmiPathWatcher *w);

#endif /* UMICOM_PATH_WATCHER_H */

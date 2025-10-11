/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/watcher_recursive.h
 * PURPOSE: Recursive watcher (monitors subdirs and restarts on root change)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WATCHER_RECURSIVE_H
#define UMICOM_WATCHER_RECURSIVE_H

#include <gio/gio.h>

typedef void (*UmiWatchEventCb)(gpointer user, const char *path);

typedef struct _UmiWatchRec UmiWatchRec;

UmiWatchRec *umi_watchrec_new(const char *root, UmiWatchEventCb cb, gpointer user);
void         umi_watchrec_set_root(UmiWatchRec *w, const char *root);
void         umi_watchrec_free(UmiWatchRec *w);

#endif /* UMICOM_WATCHER_RECURSIVE_H */

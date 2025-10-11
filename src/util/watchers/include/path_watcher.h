/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/watchers/include/path_watcher.h
 * PURPOSE: Basic watch request definition (path + recursive flag)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_PATH_WATCHER_H
#define UMICOM_PATH_WATCHER_H

#include <glib.h>  /* gboolean, gchar */

/* Minimal watch request descriptor. */
/* NOTE: This is pure data; ownership of strings clarified per API. */
typedef struct {
    gchar   *path;       /* UTF-8 path to watch (owned by caller or impl as documented) */
    gboolean recursive;  /* TRUE to watch subtree recursively                             */
} UmiPathWatch;

#endif /* UMICOM_PATH_WATCHER_H */

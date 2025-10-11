/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/fs_walk.h
 * PURPOSE: Recursive directory walk with callback
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FS_WALK_H
#define UMICOM_FS_WALK_H

#include <glib.h>      /* GDir, GFileTest, GPtrArray, gchar, etc. */

/* Caller's callback: receives (user, absolute_or_joined_path, is_dir). */
typedef void (*UmiFsVisitCb)(gpointer user, const char *path, gboolean is_dir);

/* Recursively visit 'root'. If 'root' is a file, callback is invoked once with is_dir=FALSE.
   - include_hidden: if FALSE, entries starting with '.' are skipped.
   - Order is unspecified; do not rely on it unless you sort inside your callback. */
void umi_fs_walk(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user);

#endif /* UMICOM_FS_WALK_H */

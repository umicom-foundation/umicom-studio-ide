/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/fs_walk.h
 * PURPOSE: Recursive directory walk invoking a callback per file
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FS_WALK_H
#define UMICOM_FS_WALK_H

#include <glib.h>

typedef void (*UmiFsVisitCb)(gpointer user, const char *path, gboolean is_dir);

/* Recursively walk 'root'. Hidden files are included iff include_hidden=TRUE. */
void umi_fs_walk(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user);

#endif /* UMICOM_FS_WALK_H */

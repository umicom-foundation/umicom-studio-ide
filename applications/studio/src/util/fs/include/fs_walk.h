/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/fs_walk.h
 *
 * PURPOSE:
 *   Small, allocation-friendly filesystem walker built on GLib utilities.
 *   Provides deterministic traversal (name-sorted) and an easy callback shape.
 *
 * NOTES:
 *   - Hidden entries (dotfiles) can be excluded.
 *   - Paths passed to callbacks are absolute (canonicalized) for simplicity.
 *   - No GTK dependencies; pure GLib.
 *
 * THREADING:
 *   - Synchronous; consider dispatching to a worker thread for huge trees.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FS_WALK_H
#define UMICOM_FS_WALK_H

#include <glib.h>

G_BEGIN_DECLS

/* Callback signature: receive a discovered path.
 *  - 'path'    : absolute, canonicalized path (UTF-8).
 *  - 'is_dir'  : TRUE if directory, FALSE if regular file.
 *  - 'user'    : opaque pointer forwarded from umi_fs_walk().
 */
typedef void (*UmiFsVisitCb)(const char *path, gboolean is_dir, gpointer user);

/* Walk 'root' recursively and invoke 'cb' for each entry discovered (including
 * directories). Traversal order is stable: names are sorted via g_strcmp0.
 *
 * Parameters:
 *  - include_hidden : when FALSE, dotfiles/dirs are skipped.
 *  - cb, user       : user callback + context pointer.
 *
 * Returns: TRUE on success; FALSE if 'root' did not exist or was unreadable.
 */
gboolean umi_fs_walk(const char *root, gboolean include_hidden,
                     UmiFsVisitCb cb, gpointer user);

G_END_DECLS

#endif /* UMICOM_FS_WALK_H */

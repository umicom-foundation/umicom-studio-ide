/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/file_io.h
 * PURPOSE: Atomic save helpers using temp files + rename
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_IO_H
#define UMICOM_FILE_IO_H

#include <glib.h>          /* GError, gssize, gboolean */

/* Write data atomically:
   - Writes to "path.tmp" (same directory) via g_file_set_contents().
   - Renames temp -> final (atomic on most filesystems).
   - On failure, returns FALSE and sets 'err' with errno text when available.
   - Always cleans up the temp file before returning.
   NOTE: For maximum durability (fsync), a custom open/write/fsync/rename flow would be required. */
gboolean umi_file_save_atomic(const char *path, const char *data, gssize len, GError **err);

#endif /* UMICOM_FILE_IO_H */

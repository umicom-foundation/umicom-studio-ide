/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_io.h
 * PURPOSE: Atomic save helpers using temp files + rename
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_IO_H
#define UMICOM_FILE_IO_H

#include <glib.h>

/* Write data atomically: path.tmp -> path using g_file_set_contents then rename. */
gboolean umi_file_save_atomic(const char *path, const char *data, gssize len, GError **err);

#endif /* UMICOM_FILE_IO_H */

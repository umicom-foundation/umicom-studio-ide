/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/util_path.h
 * PURPOSE: Path helpers: normalize, join, relative, is_subpath
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UTIL_PATH_H
#define UMICOM_UTIL_PATH_H

#include <glib.h>            /* GLib core types and helpers */

/* Normalize a path:
   - If 'path' is relative and 'base' is non-NULL, join base+path first.
   - Collapses "." and ".." segments when possible (pure textual; does not resolve symlinks).
   - Keeps platform separators via G_DIR_SEPARATOR_S. */
gchar *umi_path_normalize(const char *path, const char *base);

/* Return TRUE if 'child' is within 'parent' (prefix-aware, after normalization).
   - Treats exact match (parent == child) as "inside".
   - Checks path-segment boundary to avoid "/home/me" matching "/home/mega". */
gboolean umi_path_is_subpath(const char *parent, const char *child);

/* Return a relative path from base->target when possible; otherwise returns a copy of target.
   - Uses segment boundary check to avoid false positives.
   - Never returns NULL; caller must g_free() the result. */
gchar *umi_path_make_relative(const char *base, const char *target);

#endif /* UMICOM_UTIL_PATH_H */

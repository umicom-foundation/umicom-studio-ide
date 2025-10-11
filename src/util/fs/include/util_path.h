/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util_path.h
 * PURPOSE: Path helpers: normalize, join, relative, is_subpath
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UTIL_PATH_H
#define UMICOM_UTIL_PATH_H

#include <glib.h>

/* Normalize a path (collapses ./ and ../ where possible; does not touch symlinks). */
gchar *umi_path_normalize(const char *path, const char *base);

/* Return TRUE if 'child' is within 'parent' (prefix-aware, after normalization). */
gboolean umi_path_is_subpath(const char *parent, const char *child);

/* Return relative path from base->target when possible; otherwise returns a copy of target. */
gchar *umi_path_make_relative(const char *base, const char *target);

#endif /* UMICOM_UTIL_PATH_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/utf8.h
 * PURPOSE: UTF-8 utilities (validation, normalization, conversion helpers)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UTF8_H
#define UMICOM_UTF8_H

#include <glib.h>  /* gchar, gsize, gboolean */

/* HIGH-LEVEL:
 *   Provide tiny helpers to safely deal with UTF-8 strings. These wrap GLib
 *   routines with project-friendly defaults and clearly-documented behavior.
 */

/*-----------------------------------------------------------------------------
 * umi_utf8_is_valid
 *
 * PURPOSE:
 *   Return TRUE if 's' is valid UTF-8 (according to GLib). NULL is FALSE.
 *---------------------------------------------------------------------------*/
gboolean umi_utf8_is_valid(const gchar *s);

/*-----------------------------------------------------------------------------
 * umi_utf8_sanitize
 *
 * PURPOSE:
 *   Replace invalid UTF-8 sequences with U+FFFD and return a new string.
 *   If 's' is already valid, returns a duplicate of 's'.
 *
 * RETURNS:
 *   Newly-allocated string (g_free) or NULL on allocation failure.
 *---------------------------------------------------------------------------*/
gchar *umi_utf8_sanitize(const gchar *s);

/*-----------------------------------------------------------------------------
 * umi_utf8_trim
 *
 * PURPOSE:
 *   Trim leading/trailing Unicode whitespace and return a new string.
 *---------------------------------------------------------------------------*/
gchar *umi_utf8_trim(const gchar *s);

/*-----------------------------------------------------------------------------
 * umi_utf8_to_lower / umi_utf8_to_upper
 *
 * PURPOSE:
 *   Unicode-aware case conversions (locale-insensitive, GLib semantics).
 *---------------------------------------------------------------------------*/
gchar *umi_utf8_to_lower(const gchar *s);
gchar *umi_utf8_to_upper(const gchar *s);

/*-----------------------------------------------------------------------------
 * umi_utf8_from_locale
 *
 * PURPOSE:
 *   Convert a string in the current locale encoding to UTF-8. Returns a
 *   sanitized UTF-8 string on success.
 *---------------------------------------------------------------------------*/
gchar *umi_utf8_from_locale(const gchar *locale_str, GError **err);

/*-----------------------------------------------------------------------------
 * umi_utf8_to_locale
 *
 * PURPOSE:
 *   Convert UTF-8 to current locale encoding (best-effort).
 *---------------------------------------------------------------------------*/
gchar *umi_utf8_to_locale(const gchar *utf8_str, GError **err);

#endif /* UMICOM_UTF8_H */

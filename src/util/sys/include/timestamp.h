/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/timestamp.h
 * PURPOSE: Tiny helpers to format timestamps (ISO 8601, local/UTC)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TIMESTAMP_H
#define UMICOM_TIMESTAMP_H

#include <glib.h>  /* gchar, gsize, G_GNUC_WARN_UNUSED_RESULT */

/* HIGH-LEVEL:
 *   These helpers produce freshly-allocated timestamp strings for logs,
 *   filenames, status lines, etc. Callers own the returned memory.
 *
 * BEHAVIOR:
 *   - All functions return a newly-allocated NUL-terminated UTF-8 string.
 *   - On error, return NULL (never a partially-filled string).
 *   - Use g_free() to release results.
 */

/*-----------------------------------------------------------------------------
 * umi_format_time_iso8601_local
 *
 * PURPOSE:
 *   Format the current local time in ISO 8601 (e.g., "2025-10-01T12:34:56-07:00").
 *   The offset is derived from the local timezone at the time of the call.
 *
 * RETURNS:
 *   Newly-allocated string (g_free), or NULL on failure.
 *---------------------------------------------------------------------------*/
G_GNUC_WARN_UNUSED_RESULT
gchar *umi_format_time_iso8601_local(void);

/*-----------------------------------------------------------------------------
 * umi_format_time_iso8601_utc
 *
 * PURPOSE:
 *   Format the current time in UTC with "Z" suffix (e.g., "2025-10-01T19:34:56Z").
 *
 * RETURNS:
 *   Newly-allocated string (g_free), or NULL on failure.
 *---------------------------------------------------------------------------*/
G_GNUC_WARN_UNUSED_RESULT
gchar *umi_format_time_iso8601_utc(void);

/*-----------------------------------------------------------------------------
 * umi_format_time_custom
 *
 * PURPOSE:
 *   Format current time using a strftime-style pattern. Useful for filenames
 *   (e.g., "%Y%m%d-%H%M%S") or UI (e.g., "%c").
 *
 * PARAMETERS:
 *   fmt - strftime pattern; if NULL or invalid, returns NULL.
 *
 * RETURNS:
 *   Newly-allocated string (g_free), or NULL on failure.
 *---------------------------------------------------------------------------*/
G_GNUC_WARN_UNUSED_RESULT
gchar *umi_format_time_custom(const gchar *fmt);

#endif /* UMICOM_TIMESTAMP_H */

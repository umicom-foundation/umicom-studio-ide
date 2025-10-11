/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/timestamp.c
 * PURPOSE: Timestamp helpers (now, RFC3339 formatting)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>  /* GDateTime, g_date_time_*  */

#include <timestamp.h>

/* Return current time as GDateTime in local time zone. Caller owns reference. */
GDateTime *umi_now_local(void) {
    return g_date_time_new_now_local(); /* GLib API; must be unref’d by caller */
}

/* Format a GDateTime in RFC3339 (e.g., 2025-10-01T14:23:45-07:00).             */
/* Returns newly allocated string; caller must g_free().                        */
gchar *umi_time_rfc3339(GDateTime *dt) {
    if (!dt) return NULL; /* defensive */
    /* GLib can format timestamps via strftime-style patterns. %FT%T%Ez gives
     * ISO-8601 date, time, and RFC3339 offset (e.g., -07:00). */
    return g_date_time_format(dt, "%FT%T%Ez");
}
/* Format current local time in RFC3339. Returns newly allocated string.       */
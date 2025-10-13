/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/timestamp.c
 *
 * PURPOSE:
 *   Timestamp helpers (now, RFC3339 formatting) with clean ownership rules.
 *
 * API:
 *   GDateTime *umi_now_local(void);        // g_date_time_unref()
 *   gchar     *umi_time_rfc3339(GDateTime *dt);  // g_free()
 *   gchar     *umi_now_rfc3339(void);      // g_free()
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <timestamp.h>

GDateTime *umi_now_local(void) {
    return g_date_time_new_now_local();
}

gchar *umi_time_rfc3339(GDateTime *dt) {
    if (!dt) return NULL;
    return g_date_time_format(dt, "%FT%T%Ez");
}

gchar *umi_now_rfc3339(void) {
    g_autoptr(GDateTime) now = umi_now_local();
    return umi_time_rfc3339(now);
}

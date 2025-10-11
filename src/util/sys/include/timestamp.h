/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/timestamp.h
 * PURPOSE: ISO-8601 timestamp helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TIMESTAMP_H
#define UMICOM_TIMESTAMP_H

#include <glib.h>

/* Return a newly-allocated ISO-8601 timestamp string in local time */
gchar *umi_now_iso8601(void);

/* Format a timespec (seconds since epoch) to ISO-8601 */
gchar *umi_iso8601_from_time(gint64 secs);

#endif /* UMICOM_TIMESTAMP_H */

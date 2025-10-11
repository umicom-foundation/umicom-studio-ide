/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/telemetry.h
 * PURPOSE: Local counters for usage stats (no network)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TELEMETRY_H
#define UMICOM_TELEMETRY_H

#include <glib.h>

void umi_tel_inc(const char *counter);
guint64 umi_tel_get(const char *counter);
void umi_tel_reset_all(void);

#endif /* UMICOM_TELEMETRY_H */

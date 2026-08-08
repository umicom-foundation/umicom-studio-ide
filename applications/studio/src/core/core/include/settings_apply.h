/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/settings_apply.h
 * PURPOSE: Emit settings changes and apply theme live
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SETTINGS_APPLY_H
#define UMICOM_SETTINGS_APPLY_H

#include "settings_bus.h"
#include "theme.h"
#include "prefs.h"

/* After saving preferences, call to emit changes (e.g., theme) */
void umi_settings_apply_broadcast(const UmiSettings *s, GtkWindow *win);

#endif /* UMICOM_SETTINGS_APPLY_H */

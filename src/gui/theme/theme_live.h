/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/theme_live.h
 * PURPOSE: Live theme updates hooked to settings_bus
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_THEME_LIVE_H
#define UMICOM_THEME_LIVE_H

#include <gtk/gtk.h>
#include "settings_bus.h"

/* Install a handler that applies theme changes when key=='theme'. */
void umi_theme_live_install(GtkWindow *win);

#endif /* UMICOM_THEME_LIVE_H */

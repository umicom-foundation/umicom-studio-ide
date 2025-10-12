/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/settings_bus.h
 * PURPOSE: Lightweight settings signal bus
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SETTINGS_BUS_H
#define UMICOM_SETTINGS_BUS_H

#include <gtk/gtk.h>

#define UMI_TYPE_SETTINGS_BUS (umi_settings_bus_get_type())
G_DECLARE_FINAL_TYPE(UmiSettingsBus, umi_settings_bus, UMI, SETTINGS_BUS, GObject)

/* Singleton accessor */
UmiSettingsBus *umi_settings_bus_get(void);

/* Emit a change with key/value (both optional). Observers can pick based on key. */
void umi_settings_emit(UmiSettingsBus *bus, const char *key, const char *value);

#endif /* UMICOM_SETTINGS_BUS_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/prefs_ui.h
 * PURPOSE: Preferences dialog UI bound to json_store and settings_bus
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PREFS_UI_H
#define UMICOM_PREFS_UI_H

#include <gtk/gtk.h>
#include "json_store.h"
#include "../../../core/include/settings_bus.h"

/* The Preferences UI is intentionally tiny and very explicit:
 * - It shows two fields: Theme (string) and Font Size (int).
 * - It loads/saves values from a simple JSON store (human-readable file).
 * - When the user clicks Save, it also emits settings_bus changes so that
 *   other parts of the app can update live (theme + font size).
 */
typedef struct {
  GtkWidget   *dialog;
  GtkEntry    *theme_entry;
  GtkSpinButton *font_spin;
  UmiJsonStore *store;
} UmiPrefsUI;

UmiPrefsUI *umi_prefs_create(GtkWindow *parent, const char *json_path);
void        umi_prefs_show(UmiPrefsUI *ui);

#endif /* UMICOM_PREFS_UI_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs/include/prefs_ui.h
 *
 * PURPOSE:
 *   Small Preferences UI controller that loads/saves settings via a JSON
 *   store and emits live change events to the settings bus.
 *
 * DESIGN:
 *   - Public header keeps includes light and by name (no relative paths).
 *   - Implementation uses modern GTK4 (GtkWindow), not deprecated GtkDialog.
 *
 * API:
 *   typedef struct UmiPrefsUI UmiPrefsUI;
 *   UmiPrefsUI *umi_prefs_create(GtkWindow *parent, const char *json_path);
 *   void        umi_prefs_show  (UmiPrefsUI *ui);
 *   void        umi_prefs_destroy(UmiPrefsUI *ui);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PREFS_UI_H
#define UMICOM_PREFS_UI_H

#include <gtk/gtk.h>        /* public GTK types used in the API              */
#include "json_store.h"     /* UmiJsonStore (public JSON key/value store)    */
#include "settings_bus.h"   /* UmiSettingsBus events (no relative paths)     */

/* Opaque-ish controller struct (fields are public to keep C simple).        */
typedef struct UmiPrefsUI {
  GtkWindow    *window;       /* top-level window (modern, not GtkDialog)     */
  GtkEntry     *theme_entry;  /* editable theme string                        */
  GtkSpinButton*font_spin;    /* editor font size                             */
  UmiJsonStore *store;        /* settings backing store                       */
} UmiPrefsUI;

UmiPrefsUI *umi_prefs_create (GtkWindow *parent, const char *json_path);
void        umi_prefs_show   (UmiPrefsUI *ui);
void        umi_prefs_destroy(UmiPrefsUI *ui);

#endif /* UMICOM_PREFS_UI_H */

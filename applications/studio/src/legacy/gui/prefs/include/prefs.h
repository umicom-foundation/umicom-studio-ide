/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs/include/prefs.h
 *
 * PURPOSE:
 *   Higher-level Preferences API (settings model + modal helpers). This is
 *   optional if you use prefs_ui.h directly; provided here for completeness.
 *
 * DESIGN:
 *   - Small POD settings struct with loader/saver helpers.
 *   - UI helper functions for modal presentation.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_PREFS_H
#define UMI_PREFS_H

#include <glib.h>
#include <gtk/gtk.h>

typedef struct UmiSettings {
  char    *theme;                 /* theme name or NULL for default             */
  int      font_size;             /* editor font size in points                 */
  gboolean autosave_enabled;      /* optional future field                      */
  int      autosave_interval_sec; /* optional future field                      */
} UmiSettings;

UmiSettings *umi_settings_load(void);
gboolean     umi_settings_save(const UmiSettings *s);
void         umi_settings_free(UmiSettings *s);

/* Optional controller wrapper (separate from prefs_ui.h controller).         */
typedef struct UmiPrefs UmiPrefs;

UmiPrefs  *umi_prefs_new(void);
void       umi_prefs_free(UmiPrefs *p);
void       umi_prefs_set_parent(UmiPrefs *p, GtkWindow *parent);
GtkWidget *umi_prefs_widget(UmiPrefs *p);
gboolean   umi_prefs_show_modal(UmiPrefs *p, GtkWindow *parent);

#endif /* UMI_PREFS_H */

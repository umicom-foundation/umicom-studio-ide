/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs.h
 * PURPOSE: Preferences UI API + persistent settings model.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_PREFS_H
#define UMI_PREFS_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* -----------------------------
 * Persistent settings (model)
 * ----------------------------- */
typedef struct UmiSettings {
  /* theme: "light", "dark", etc. */
  char     *theme;
  /* editor font size in points */
  int       font_size;
  /* tool paths (may be empty) */
  char     *umicc_path;
  char     *uaengine_path;
  char     *ripgrep_path;
  /* autosave configuration */
  gboolean  autosave_enabled;
  int       autosave_interval_sec;
} UmiSettings;

/* Load settings from disk (uses project default location inside function). */
UmiSettings *umi_settings_load(void);
/* Save settings to disk (project default path). Matches prefs.c one-arg signature. */
gboolean     umi_settings_save(const UmiSettings *s);
/* Free settings and owned strings. */
void         umi_settings_free(UmiSettings *s);

/* -----------------------------
 * Preferences UI (controller)
 * ----------------------------- */
typedef struct UmiPrefs UmiPrefs;

/* Construction and lifetime */
UmiPrefs  *umi_prefs_new(void);
void       umi_prefs_free(UmiPrefs *p);

/* Parent wiring and widget access */
void       umi_prefs_set_parent(UmiPrefs *p, GtkWindow *parent);
GtkWidget *umi_prefs_widget(UmiPrefs *p);

/* UX helpers */
gboolean   umi_prefs_show_modal(UmiPrefs *p, GtkWindow *parent);

G_END_DECLS

#endif /* UMI_PREFS_H */

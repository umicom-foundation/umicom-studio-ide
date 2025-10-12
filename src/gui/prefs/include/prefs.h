/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs.h
 * PURPOSE: Preferences UI API + persistent settings model.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_PREFS_H
#define UMI_PREFS_H

#include <glib.h>   /* GLib base types like gboolean, gchar, gsize         */
#include <gtk/gtk.h>/* GTK widget types used by the Preferences UI API      */

G_BEGIN_DECLS /* Ensure C linkage when included from C++ sources */

/* ---------------------------------------------------------------------------
 * UmiSettings:
 * A plain-old-data model that mirrors what we persist to disk (e.g. JSON).
 * Each field is owned by the struct unless stated otherwise; free with
 * umi_settings_free() to release strings allocated by the loader.
 *---------------------------------------------------------------------------*/
typedef struct UmiSettings {
  /* theme name (e.g. "light", "dark", "system"). May be NULL -> use default. */
  char     *theme;

  /* Editor font size in points; >= 8 is typical. 0 -> use default.          */
  int       font_size;

  /* Optional tool paths (NULL or empty means: find via PATH at runtime).     */
  char     *umicc_path;
  char     *uaengine_path;
  char     *ripgrep_path;

  /* Autosave prefs: if enabled, editor saves periodically every interval sec. */
  gboolean  autosave_enabled;
  int       autosave_interval_sec;
} UmiSettings;

/* ---------------------------------------------------------------------------
 * umi_settings_load:
 * Loads settings from the project’s default configuration path and returns a
 * newly allocated UmiSettings* (or a sane-defaults struct if file is absent).
 * Caller owns the returned pointer and must call umi_settings_free().
 *---------------------------------------------------------------------------*/
UmiSettings *umi_settings_load(void);

/* ---------------------------------------------------------------------------
 * umi_settings_save:
 * Persists the provided settings to the default configuration path.
 * Returns TRUE on success; FALSE otherwise (e.g., I/O error).
 *---------------------------------------------------------------------------*/
gboolean     umi_settings_save(const UmiSettings *s);

/* ---------------------------------------------------------------------------
 * umi_settings_free:
 * Releases all owned strings within UmiSettings and then frees the struct.
 * Accepts NULL for convenience (no-op).
 *---------------------------------------------------------------------------*/
void         umi_settings_free(UmiSettings *s);

/* -----------------------------
 * Preferences UI (controller)
 * -----------------------------
 * The UI layer is intentionally thin: construct with umi_prefs_new(),
 * pack the widget via umi_prefs_widget(), and optionally present it modally
 * using umi_prefs_show_modal(). Parent wiring is optional but recommended.
 */
typedef struct UmiPrefs UmiPrefs;  /* Opaque controller struct */

/* Create/destroy the preferences dialog controller (not yet shown). */
UmiPrefs  *umi_prefs_new(void);
void       umi_prefs_free(UmiPrefs *p);

/* Wire an optional transient parent window for better window management.     */
void       umi_prefs_set_parent(UmiPrefs *p, GtkWindow *parent);

/* Return the top-level GTK widget for packing in custom containers.          */
GtkWidget *umi_prefs_widget(UmiPrefs *p);

/* Convenience: show as a modal dialog; returns TRUE if user accepted/saved.  */
gboolean   umi_prefs_show_modal(UmiPrefs *p, GtkWindow *parent);

G_END_DECLS

#endif /* UMI_PREFS_H */

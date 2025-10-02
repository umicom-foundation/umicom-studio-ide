/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/prefs.h
 * PURPOSE: Settings model + load/save + GTK dialog
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PREFS_H
#define UMICOM_PREFS_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* User-facing settings for the IDE (persisted as JSON). */
typedef struct {
  gchar *theme;                 /* "light" or "dark" */
  int    font_size;             /* editor font size */
  gchar *umicc_path;            /* toolchain: umicc */
  gchar *uaengine_path;         /* UAEngine binary path (optional) */
  gchar *ripgrep_path;          /* override rg path; empty uses discovery */
  gboolean autosave_enabled;    /* autosave toggle */
  guint    autosave_interval_sec; /* autosave period */
} UmiSettings;

/* Load or create defaults. Caller owns returned struct (free with umi_settings_free). */
UmiSettings* umi_settings_load(void);

/* Save settings to disk (returns FALSE on I/O error). */
gboolean     umi_settings_save(const UmiSettings *s);

/* Destroy settings and free owned strings. */
void         umi_settings_free(UmiSettings *s);

/* Create and run a modal Preferences dialog. If 'in_out' is non-NULL,
 * fields are both read and written back on OK. */
GtkWidget*   umi_prefs_dialog_new(GtkWindow *parent, UmiSettings *in_out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_PREFS_H */

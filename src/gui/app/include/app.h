/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/include/theme.h
 * PURPOSE: Public theme APIs (loading CSS, applying theme, reading presets)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *
 * Plain-English notes:
 * - This header exposes tiny, easy-to-understand functions that do 2 things:
 *   (1) apply the app's CSS theme to a GTK window/display, and
 *   (2) read the embedded JSON preset data as a string.
 * - We keep this API small so it's easy to use and hard to misuse.
 * - Resource files are compiled into the app via ustudio.gresource.xml.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_H
#define UMICOM_THEME_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * umi_theme_apply:
 * @win: (nullable): A window whose display we will style. If NULL, we will
 *                   style the default display (if available).
 *
 * Loads the app CSS from the embedded resource and applies it to the display.
 * (resources) and tell GTK: style so everything looks
 * consistent and nice.
 */
void umi_theme_apply(GtkWindow *win);

/**
 * umi_theme_load_presets_json:
 * @len_out: (out) (nullable): If not NULL, receives the number of bytes.
 *
 * Reads the embedded JSON preset file and returns a newly-allocated,
 * NUL-terminated string with the file contents. Caller must g_free() it.
 * If anything goes wrong, returns NULL.
 */
gchar *umi_theme_load_presets_json(gsize *len_out);

G_END_DECLS

#endif /* UMICOM_THEME_H */

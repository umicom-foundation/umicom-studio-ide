/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/include/theme.h
 * PURPOSE: Theme application — declarations
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_H
#define UMICOM_THEME_H

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

/*
 * Plain-English overview:
 * - umi_theme_apply(win, theme_name) loads our app-wide CSS from GResource
 *   and (optionally) lets us switch by name later. Today we always load
 *   styles/app.css and ignore theme_name if there is only one CSS.
 * - umi_theme_apply_default(win) is a friendly helper that calls apply with
 *   a default/NULL name.
 * - umi_theme_load_presets() returns a COPY of the parsed JSON node from
 *   styles/theme_presets.json (caller must json_node_free()).
 * - umi_theme_shutdown() frees internal singletons for leak-check runs.
 */

G_BEGIN_DECLS

void      umi_theme_apply           (GtkWindow *win, const char *theme_name);
void      umi_theme_apply_default   (GtkWindow *win);
JsonNode *umi_theme_load_presets    (void);
void      umi_theme_shutdown        (void);

G_END_DECLS
#endif /* UMICOM_THEME_H */
/*----------------------------------------------------------------------------
 * Helper: Resolve a theme name to a CSS resource path by looking it up
 * in the theme presets JSON manifest.
 * Returns: newly allocated string (g_free) or NULL if not found/empty.
 *----------------------------------------------------------------------------*/
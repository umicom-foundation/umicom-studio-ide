/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/include/theme.h
 *
 * PURPOSE:
 *   Declarations for theme application and preset handling. Applies base CSS
 *   from GResource and optionally a named preset CSS resolved from a JSON
 *   manifest compiled into the binary.
 *
 * API:
 *   void      umi_theme_apply         (GtkWindow *win, const char *theme_name);
 *   void      umi_theme_apply_default (GtkWindow *win);              // helper
 *   JsonNode *umi_theme_load_presets  (void);                        // caller frees via json_node_free()
 *   void      umi_theme_shutdown      (void);                        // for leak-checkers; no-op otherwise
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_H
#define UMICOM_THEME_H

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

void      umi_theme_apply         (GtkWindow *win, const char *theme_name);
void      umi_theme_apply_default (GtkWindow *win);
JsonNode *umi_theme_load_presets  (void);
void      umi_theme_shutdown      (void);

G_END_DECLS
#endif /* UMICOM_THEME_H */

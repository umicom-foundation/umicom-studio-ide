/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/include/theme_live.h
 *
 * PURPOSE:
 *   Live overrides on top of the base theme using a USER-priority
 *   GtkCssProvider. Useful for quick accent changes at runtime.
 *
 * IMPORTANT:
 *   GTK CSS **does not** support browser-style CSS variables `var(--x)`.
 *   Use @define-color or utility classes instead (we do both in the .c).
 *
 * API:
 *   void umi_theme_live_set_accent   (const char *rgba_css);
 *   void umi_theme_live_clear_overrides(void);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_LIVE_H
#define UMICOM_THEME_LIVE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void umi_theme_live_set_accent    (const char *rgba_css);
void umi_theme_live_clear_overrides(void);

G_END_DECLS

#endif /* UMICOM_THEME_LIVE_H */

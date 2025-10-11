/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/theme.h
 * PURPOSE: Light/dark theme helper + CSS overlay
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_THEME_H
#define UMICOM_THEME_H

#include <gtk/gtk.h>

/* Apply "light" or "dark" theme and attach our minimal CSS overlay.
 * Unknown names default to light. */
void umi_theme_apply(GtkWindow *win, const char *name);

/* Return a singleton CSS provider to tweak styles globally. */
GtkCssProvider* umi_theme_css(void);

#endif /* UMICOM_THEME_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/theme.h
 *
 * PURPOSE:
 *   Core-visible theme API shim that forwards to the GUI theme module.
 *   This header exists to provide a stable include path for core code.
 *
 * DESIGN:
 *   - Only declarations are provided here.
 *   - Implementations live in the GUI theme directory (src/gui/theme/).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_H
#define UMICOM_THEME_H

#include <gtk/gtk.h>
G_BEGIN_DECLS
/* Apply a named theme to the given window. */
void umi_theme_apply(GtkWindow *win, const char *theme_name);

void umi_theme_apply_brand_css(GtkWidget *root_window);

/* Apply the default theme to the given window. */
void umi_theme_apply_default(GtkWindow *win);
G_END_DECLS
#endif /* UMI_THEME_H */
/*  END OF FILE */
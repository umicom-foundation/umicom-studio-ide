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
 *   - Implementations live in src/gui/theme/*.c.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_THEME_H
#define UMI_THEME_H

#include <gtk/gtk.h>

void umi_theme_apply(GtkWindow *win, const char *theme_name);
void umi_theme_apply_default(GtkWindow *win);

#endif /* UMI_THEME_H */
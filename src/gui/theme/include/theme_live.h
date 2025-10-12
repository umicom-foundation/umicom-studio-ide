/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/include/theme_live.h
 * PURPOSE: Live theming overrides (e.g., change accent at runtime)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *
 * Notes:
 * - Lightweight helpers that layer quick CSS tweaks over the base theme.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_LIVE_H
#define UMICOM_THEME_LIVE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * umi_theme_live_set_accent:
 * @rgba: CSS color string, e.g. "#4a90e2" or "rgba(74,144,226,1.0)".
 *
 * Applies a high-priority CSS override that sets a --umi-accent variable.
 * Your CSS can reference it like:  color: var(--umi-accent);
 */
void umi_theme_live_set_accent(const char *rgba);

/**
 * umi_theme_live_clear_overrides:
 *
 * Clears live overrides applied by this module, restoring the base theme.
 */
void umi_theme_live_clear_overrides(void);

G_END_DECLS

#endif /* UMICOM_THEME_LIVE_H */
/*----------------------------------------------------------------------------
 * Helper: Resolve a theme name to a CSS resource path by looking it up
 * in the theme presets JSON manifest.
 * Returns: newly allocated string (g_free) or NULL if not found/empty.
 *----------------------------------------------------------------------------*/
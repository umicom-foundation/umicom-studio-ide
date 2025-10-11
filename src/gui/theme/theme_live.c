/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/theme_live.c
 * PURPOSE: Live theming helpers layered on top of the base theme
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *
 * Overview:
 * - Maintains one USER-priority GtkCssProvider for quick runtime overrides.
 * - Typical use: update a CSS custom property used across the app.
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include "include/theme_live.h"

/* Single provider used for live overrides. */
static GtkCssProvider *s_live_provider = NULL;

/* Ensure we have a provider and attach it to the default display. */
static void
umi_theme_live_ensure_provider(void)
{
    if (s_live_provider)
        return;

    s_live_provider = gtk_css_provider_new();

    GdkDisplay *display = gdk_display_get_default();
    if (display) {
        gtk_style_context_add_provider_for_display(
            display,
            GTK_STYLE_PROVIDER(s_live_provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER  /* higher than application */
        );
    } else {
        g_warning("USIDE: No default display for live theme overrides.");
    }
}

void
umi_theme_live_set_accent(const char *rgba)
{
    umi_theme_live_ensure_provider();

    if (!s_live_provider)
        return;

    if (!rgba || !*rgba)
        rgba = "#4a90e2"; /* default */

    gchar *css = g_strdup_printf(":root { --umi-accent: %s; }\n", rgba);

    GError *err = NULL;
    gtk_css_provider_load_from_data(s_live_provider, css, -1, &err);
    if (err) {
        g_warning("USIDE: live accent CSS error: %s", err->message);
        g_clear_error(&err);
    }

    g_free(css);
}

void
umi_theme_live_clear_overrides(void)
{
    umi_theme_live_ensure_provider();

    if (!s_live_provider)
        return;

    GError *err = NULL;
    gtk_css_provider_load_from_data(s_live_provider, "", 0, &err);
    if (err) {
        g_warning("USIDE: live override clear error: %s", err->message);
        g_clear_error(&err);
    }
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/theme_live.c
 * PURPOSE: Live theming helpers layered on top of the base theme
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 2025-10-12
 * License: MIT
 *
 * Overview (loose coupling):
 *  - This module owns a single GtkCssProvider (USER priority) used for quick
 *    runtime overrides (e.g., changing an “accent” color on the fly).
 *  - It depends only on GTK/GLib and its own public header. No other project
 *    headers are needed.
 *  - Uses modern GTK-4 API: gtk_css_provider_load_from_string().
 *
 * Notes:
 *  - We intentionally keep this provider separate from any base theme so that
 *    clearing overrides puts the app back to the baseline instantly.
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "theme_live.h"

/*-----------------------------------------------------------------------------
 * Static state owned by this module
 *---------------------------------------------------------------------------*/
static GtkCssProvider *s_live_provider = NULL;  /* our USER-priority provider */
static gboolean        s_installed     = FALSE; /* whether we've attached it */

/*-----------------------------------------------------------------------------
 * umi_theme_live_ensure_provider:
 * Lazily create the USER-priority provider and attach it to the default display.
 *---------------------------------------------------------------------------*/
static void
umi_theme_live_ensure_provider(void)
{
    if (!s_live_provider) {
        s_live_provider = gtk_css_provider_new();
    }

    if (!s_installed) {
        /* Attach as USER priority so it sits above app/theme defaults. */
        GdkDisplay *disp = gdk_display_get_default();
        if (disp) {
            gtk_style_context_add_provider_for_display(
                disp,
                GTK_STYLE_PROVIDER(s_live_provider),
                GTK_STYLE_PROVIDER_PRIORITY_USER
            );
            s_installed = TRUE;
        }
    }
}

/*-----------------------------------------------------------------------------
 * umi_theme_live_set_accent:
 * @rgba: CSS color string, e.g. "#4a90e2" or "rgba(74,144,226,1.0)".
 *
 * Loads a tiny CSS snippet into our USER-level provider that can be used by
 * your widgets. Two simple examples are shown below:
 *
 *  (A) Via a custom class you can add to top-level/root widgets:
 *      .umi-accent { color: <rgba>; }
 *      -> Add class "umi-accent" to a container or widget that should adopt it.
 *
 *  (B) If you already have CSS expecting some custom name:
 *      @define-color umi_accent <rgba>;
 *      -> Your theme CSS may then refer to @umi_accent in properties.
 *
 * GTK CSS does not support CSS Variables (var(--...)) like browsers do, so
 * prefer either a custom class or @define-color.
 *---------------------------------------------------------------------------*/
void
umi_theme_live_set_accent(const char *rgba)
{
    umi_theme_live_ensure_provider();

    if (!s_live_provider || !rgba || !*rgba)
        return;

    /* Choose one of the patterns. Below we publish BOTH to make it flexible:
     *  - @define-color name …  (classic GTK color alias)
     *  - a utility class .umi-accent that sets foreground color
     */
    char css[512];
    g_snprintf(css, sizeof css,
               "@define-color umi_accent %s;\n"
               ".umi-accent { color: %s; }\n",
               rgba, rgba);

    /* GTK4 modern API: no GError here, just pass the full CSS string. */
    gtk_css_provider_load_from_string(s_live_provider, css);
}

/*-----------------------------------------------------------------------------
 * umi_theme_live_clear_overrides:
 * Clears (overwrites with empty CSS) our USER-level provider, effectively
 * restoring the base theme visuals.
 *---------------------------------------------------------------------------*/
void
umi_theme_live_clear_overrides(void)
{
    umi_theme_live_ensure_provider();

    if (!s_live_provider)
        return;

    /* Empty CSS string wipes our live overrides. */
    gtk_css_provider_load_from_string(s_live_provider, "");
}

/*-----------------------------------------------------------------------------
 * Optional cleanup helper (not part of the public header on purpose).
 * If you ever want to hard-destroy the provider (e.g., during shutdown),
 * you can expose a function that calls this safely.
 *---------------------------------------------------------------------------*/
static void
umi_theme_live_destroy_provider(void)
{
    if (s_live_provider) {
        /* Detach is not strictly necessary; destroying the provider removes it. */
        g_clear_object(&s_live_provider);
    }
    s_installed = FALSE;
}

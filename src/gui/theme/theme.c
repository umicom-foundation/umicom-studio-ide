/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/theme.c
 *
 * PURPOSE:
 *   Minimal theme application stubs wired to GTK4. Provides the public
 *   functions declared in core/include/theme.h so core code can request a
 *   theme change without depending on GUI internals.
 *
 * DESIGN:
 *   - Keep it safe & no-op friendly: if theme_name is NULL/unknown, fall back
 *     to default. This avoids link/runtime failures when theming assets are
 *     absent.
 *   - No removal of comments or header banners; only minimal code added.
 *
 * NOTE [2025-10-14]:
 *   Replaced deprecated gtk_css_provider_load_from_data() with
 *   gtk_css_provider_load_from_string() to silence GTK4 deprecation warnings.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gtk/gtk.h>
#include "theme.h"  /* public declarations */

/* Simple helper: apply a tiny CSS snippet to the display. In the future this
 * can load full theme CSS from resources. For now we remain conservative. */
static void apply_css_to_display(const char *css)
{
    if (!css) return;
    GtkCssProvider *prov = gtk_css_provider_new();

    /* GTK4 modern API (non-deprecated) */
    gtk_css_provider_load_from_string(prov, css);

    GdkDisplay *display = gdk_display_get_default();
    if (display) {
        gtk_style_context_add_provider_for_display(
            display, GTK_STYLE_PROVIDER(prov), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
    g_object_unref(prov);
}

void umi_theme_apply(GtkWindow *win, const char *theme_name)
{
    /* We intentionally tolerate NULLs — no crashes in headless/test runs. */
    (void)win;

    /* Very conservative defaults: light/dark toggle via CSS accent only.
     * Extend later to load resources with glib-compile-resources. */
    if (theme_name && g_strcmp0(theme_name, "dark") == 0) {
        apply_css_to_display(
            ":root { color-scheme: dark; }\n"
            "window { } /* reserved for future theme rules */\n");
    } else {
        /* default / light */
        apply_css_to_display(
            ":root { color-scheme: light; }\n"
            "window { } /* reserved for future theme rules */\n");
    }
}

void umi_theme_apply_default(GtkWindow *win)
{
    umi_theme_apply(win, NULL);
}
/*  END OF FILE */

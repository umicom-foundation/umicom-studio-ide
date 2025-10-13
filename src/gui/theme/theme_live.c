/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/theme_live.c
 *
 * PURPOSE:
 *   Provide a USER-priority GtkCssProvider to layer quick runtime tweaks
 *   (e.g., accent color) above the base app CSS.
 *
 * DESIGN:
 *   - Lazily creates and attaches a single provider per process.
 *   - Uses gtk_css_provider_load_from_string() (GTK4).
 *   - Provides both @define-color and a .umi-accent class for flexibility.
 *
 * SECURITY:
 *   - No unbounded string ops; g_snprintf for CSS assembly.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "theme_live.h"

static GtkCssProvider *s_live_provider = NULL;
static gboolean        s_installed     = FALSE;

static void ensure_provider(void)
{
  if (!s_live_provider)
    s_live_provider = gtk_css_provider_new();

  if (!s_installed) {
    GdkDisplay *disp = gdk_display_get_default();
    if (disp) {
      gtk_style_context_add_provider_for_display(
          disp, GTK_STYLE_PROVIDER(s_live_provider),
          GTK_STYLE_PROVIDER_PRIORITY_USER);
      s_installed = TRUE;
    }
  }
}

void umi_theme_live_set_accent(const char *rgba_css)
{
  ensure_provider();
  if (!s_live_provider || !rgba_css || !*rgba_css) return;

  char css[512];
  g_snprintf(css, sizeof css,
             "@define-color umi_accent %s;\n"
             ".umi-accent { color: %s; }\n",
             rgba_css, rgba_css);

  gtk_css_provider_load_from_string(s_live_provider, css);
}

void umi_theme_live_clear_overrides(void)
{
  ensure_provider();
  if (!s_live_provider) return;
  gtk_css_provider_load_from_string(s_live_provider, "");
}

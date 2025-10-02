/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/theme.c
 * PURPOSE: Implementation of theme helper and CSS overlay
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "theme.h"

static GtkCssProvider *g_css = NULL;

GtkCssProvider* umi_theme_css(void){
  if(!g_css){
    g_css = gtk_css_provider_new();
    const char *css =
      "/* Umicom Studio IDE minimal CSS overlay */\n"
      ":root {{}}\n"
      "textview {{ font-family: monospace; font-size: 12pt; }}\n"
      ".error {{ color: red; }}\n"
      ".warning {{ color: #e0a000; }}\n";
    gtk_css_provider_load_from_data(g_css, css, -1);
  }
  return g_css;
}

void umi_theme_apply(GtkWindow *win, const char *name){
  if(!win) return;
  GtkStyleContext *ctx = gtk_widget_get_style_context(GTK_WIDGET(win));
  gtk_style_context_add_provider_for_display(
    gdk_display_get_default(),
    GTK_STYLE_PROVIDER(umi_theme_css()),
    GTK_STYLE_PROVIDER_PRIORITY_USER
  );

  gboolean dark = (name && g_ascii_strcasecmp(name, "dark")==0);
  GtkSettings *sett = gtk_settings_get_default();
  g_object_set(sett, "gtk-application-prefer-dark-theme", dark, NULL);
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/theme_live.c
 * PURPOSE: Implementation: hook settings changes to theme_apply
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "theme_live.h"
#include "theme.h"

static void on_changed(UmiSettingsBus *bus, const char *key, const char *value, gpointer user){
  (void)bus;
  GtkWindow *win = GTK_WINDOW(user);
  if(g_strcmp0(key,"theme")==0){
    umi_theme_apply(win, value);
  }
}

void umi_theme_live_install(GtkWindow *win){
  UmiSettingsBus *bus = umi_settings_bus_get();
  g_signal_connect(bus, "changed", G_CALLBACK(on_changed), win);
}

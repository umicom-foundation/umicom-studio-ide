/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/settings_apply.c
 * PURPOSE: Implementation of settings broadcast
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/settings_apply.h"

void umi_settings_apply_broadcast(const UmiSettings *s, GtkWindow *win){
  UmiSettingsBus *bus = umi_settings_bus_get();
  if(s && s->theme){
    umi_settings_emit(bus, "theme", s->theme);
    if(win) umi_theme_apply(win, s->theme);
  }
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.c
 * PURPOSE: Fill the keyboard callback table (connects to App methods)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include "app_actions.h"
#include "app.h"
#include "status_bar.h"
#include "editor.h"
#include "search_panel.h"

static void act_palette(gpointer u){
  UmiApp *ua = (UmiApp*)u;
  if(ua && ua->status) umi_status_flash(ua->status, "Command palette not implemented yet", 1200);
}
static void act_save(gpointer u){
  UmiApp *ua = (UmiApp*)u; if(!ua) return;
  GError *e=NULL; umi_editor_save(ua->ed, &e); if(e){ g_error_free(e); }
}
static void act_save_as(gpointer u){
  UmiApp *ua = (UmiApp*)u; if(!ua) return;
  GError *e=NULL; umi_editor_save_as(ua->ed, &e); if(e){ g_error_free(e); }
}
static void act_run(gpointer u){
  UmiApp *ua = (UmiApp*)u;
  if(ua && ua->status) umi_status_flash(ua->status, "Run: not wired yet", 1200);
}
static void act_stop(gpointer u){
  UmiApp *ua = (UmiApp*)u;
  if(ua && ua->status) umi_status_flash(ua->status, "Stop: not wired yet", 1200);
}
static void act_focus_search(gpointer u){
  UmiApp *ua = (UmiApp*)u; if(!ua) return;
  if(ua->status) umi_status_flash(ua->status, "Focus: Search", 900);
  /* If your search panel exposes a focus function, call it here. */
}

void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out){
  (void)app;
  if(!out) return;
  out->palette = act_palette;
  out->save = act_save;
  out->save_as = act_save_as;
  out->run = act_run;
  out->stop = act_stop;
  out->focus_search = act_focus_search;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.c
 * PURPOSE: Implement keymap callbacks for palette, save, run, etc.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "app_actions.h"
#include "keymap.h"
#include "status_util.h"

/* Trampolines (no args) */
static void act_palette(gpointer u){
  UmiApp *ua = umi_app_handle((GtkApplication*)u);
  if(!ua) return;
  (void)ua; /* palette is invoked from cmd in app.c; placeholder */
}

static void act_save(gpointer u){
  UmiApp *ua = umi_app_handle((GtkApplication*)u);
  if(!ua) return;
  GError *e=NULL;
  if(!umi_editor_save(umi_app_editor(ua), &e) && e){ g_error_free(e); }
}

static void act_save_as(gpointer u){
  UmiApp *ua = umi_app_handle((GtkApplication*)u);
  if(!ua) return;
  /* For simplicity, write to 'untitled.txt' in CWD if no path. */
  UmiEditor *ed = umi_app_editor(ua);
  const char *path = (ed && ed->current_file)? ed->current_file : "untitled.txt";
  GError *e=NULL; if(!umi_editor_save_as(ed, path, &e) && e){ g_error_free(e); }
}

static void act_run(gpointer u){
  /* run/stop already wired in app.c via command palette entries */
  (void)u;
}

static void act_stop(gpointer u){ (void)u; }

static void act_focus_search(gpointer u){
  UmiApp *ua = umi_app_handle((GtkApplication*)u);
  if(!ua || !ua->ed || !ua->ed->search) return;
  umi_search_panel_focus_query(ua->ed->search);
}

void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out){
  if(!out) return;
  out->palette = act_palette;
  out->save = act_save;
  out->save_as = act_save_as;
  out->run = act_run;
  out->stop = act_stop;
  out->focus_search = act_focus_search;
  out->user = app;
}

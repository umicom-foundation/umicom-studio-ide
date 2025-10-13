/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app_actions.c
 *
 * PURPOSE:
 *   Application-level action callbacks for menus & keymap. This module keeps
 *   cross-module coupling *very* low by using weak references for optional
 *   features (editor, run pipeline). Pure C, no CSS, no UI dependencies here.
 *
 * SECURITY/ROBUSTNESS:
 *   - No unbounded string formatting.
 *   - All pointers are checked before use.
 *   - Weak symbols let this compile even when optional modules are missing.
 *
 * API:
 *   void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "app_actions.h"  /* our declaration (includes keymap.h) */
#include "app.h"          /* UmiApp struct & accessors           */

/* Optional cross-module features via weak symbols (portable guards).       */
/* GNU/Clang: use weak references; MSVC: fall back to NULL function ptrs.   */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) gboolean umi_editor_save   (struct _UmiEditor *ed, GError **err);
__attribute__((weak)) gboolean umi_editor_save_as(struct _UmiEditor *ed, GError **err);
__attribute__((weak)) gboolean umi_run_pipeline_start(gpointer out, gpointer problems, GError **err);
__attribute__((weak)) void     umi_run_pipeline_stop(void);
#else
gboolean (*umi_editor_save)   (struct _UmiEditor*, GError**) = NULL;
gboolean (*umi_editor_save_as)(struct _UmiEditor*, GError**) = NULL;
gboolean (*umi_run_pipeline_start)(gpointer,gpointer,GError**) = NULL;
void     (*umi_run_pipeline_stop)(void) = NULL;
#endif

/* Small helper to log a line (kept UI-agnostic). */
static inline void log_info(const char *msg)
{
  if (msg && *msg) g_message("%s", msg);
}

/* Run/Stop ------------------------------------------------------------------*/

static void action_run(gpointer user)
{
  (void)user;
  if (umi_run_pipeline_start) {
    GError *err = NULL;
    if (!umi_run_pipeline_start(NULL, NULL, &err)) {
      if (err) { g_warning("Run failed: %s", err->message); g_clear_error(&err); }
    } else {
      log_info("Run started");
    }
  } else {
    log_info("Run not available (runner not linked)");
  }
}

static void action_stop(gpointer user)
{
  (void)user;
  if (umi_run_pipeline_stop) {
    umi_run_pipeline_stop();
    log_info("Run stopped");
  } else {
    log_info("Stop not available (runner not linked)");
  }
}

/* Save / Save As ------------------------------------------------------------*/

static void action_save(gpointer user)
{
  UmiApp *ua = (UmiApp *)user;
  if (!ua) { log_info("Save: no app context"); return; }

  struct _UmiEditor *ed = umi_app_editor(ua);
  if (!ed) { log_info("Save: no editor"); return; }

  if (umi_editor_save) {
    GError *err = NULL;
    if (!umi_editor_save(ed, &err)) {
      if (err) { g_warning("Save failed: %s", err->message); g_clear_error(&err); }
    } else {
      log_info("Saved");
    }
  } else {
    log_info("Save not available (editor not linked)");
  }
}

static void action_save_as(gpointer user)
{
  UmiApp *ua = (UmiApp *)user;
  if (!ua) { log_info("Save As: no app context"); return; }

  struct _UmiEditor *ed = umi_app_editor(ua);
  if (!ed) { log_info("Save As: no editor"); return; }

  if (umi_editor_save_as) {
    GError *err = NULL;
    if (!umi_editor_save_as(ed, &err)) {
      if (err) { g_warning("Save As failed: %s", err->message); g_clear_error(&err); }
    } else {
      log_info("Saved As");
    }
  } else {
    log_info("Save As not available (editor not linked)");
  }
}

/* Placeholders to keep keymap complete (safe no-ops). */
static void action_palette(gpointer user)      { (void)user; log_info("Palette (not implemented yet)"); }
static void action_focus_search(gpointer user) { (void)user; log_info("Focus search (not implemented yet)"); }

/* Public wiring -------------------------------------------------------------*/

void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out)
{
  (void)app;
  if (!out) return;
  out->palette      = action_palette;
  out->save         = action_save;
  out->save_as      = action_save_as;
  out->run          = action_run;
  out->stop         = action_stop;
  out->focus_search = action_focus_search;
}

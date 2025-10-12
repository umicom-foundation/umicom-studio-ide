/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app_actions.c
 * PURPOSE: Application-level action callbacks used by menus & keymap.
 *          Self-contained (pure C + GLib/GTK) — no cross-module headers.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>

#include "include/app_actions.h"  /* public prototypes for this module */
#include "include/app.h"          /* UmiApp struct & accessors for this module */

/* ────────────────────────────────────────────────────────────────────────── */
/* Optional cross-module features via weak symbols
 *
 * We declare these as weak so this file compiles/links even when the
 * corresponding modules are not part of the build yet.
 *
 * - Editor actions live under the editor module.
 * - Run/Stop live under the core/run pipeline module.
 */
__attribute__((weak)) gboolean umi_editor_save(struct _UmiEditor *ed, GError **err);
__attribute__((weak)) gboolean umi_editor_save_as(struct _UmiEditor *ed, GError **err);

__attribute__((weak)) gboolean umi_run_pipeline_start(gpointer out, gpointer problems, GError **err);
__attribute__((weak)) void     umi_run_pipeline_stop(void);

/* ────────────────────────────────────────────────────────────────────────── */
/* Small helper: log a message (placeholder for future status bar integration)
 * We intentionally DO NOT depend on a status bar or UI component here.
 */
static inline void
log_info(const char *msg)
{
    if (msg && *msg) g_message("%s", msg);
}

/* ────────────────────────────────────────────────────────────────────────── */
/* Action implementations
 * All actions accept a generic userdata pointer. We expect it to be UmiApp*.
 * If the editor/run modules are not linked, weak hooks will be NULL and these
 * actions will simply log — keeping the app responsive and build-green.
 */

static void
action_run(gpointer user)
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

static void
action_stop(gpointer user)
{
    (void)user;
    if (umi_run_pipeline_stop) {
        umi_run_pipeline_stop();
        log_info("Run stopped");
    } else {
        log_info("Stop not available (runner not linked)");
    }
}

static void
action_save(gpointer user)
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

static void
action_save_as(gpointer user)
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

static void
action_palette(gpointer user)
{
    (void)user;
    log_info("Palette (not implemented yet)");
}

static void
action_focus_search(gpointer user)
{
    (void)user;
    log_info("Focus search (not implemented yet)");
}

/* ────────────────────────────────────────────────────────────────────────── */
/* Public API: fill the keymap callback table for the app shell.
 * This keeps wiring centralized and self-contained within the app module.
 */
void
umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out)
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
/*--- end of file ---*/
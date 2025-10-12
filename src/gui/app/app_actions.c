/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.c
 * PURPOSE: Application-level action callbacks and helpers used by menus & UI
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>

/* Local header */
#include "app_actions.h"

/* Core & GUI headers */
#include "../../include/status_bar.h"          /* UmiStatusBar, umi_status_bar_inner */
#include "../../util/log/include/status_util.h"         /* umi_status_flash */
#include <umicom/editor.h>              /* UmiEditor, umi_editor_save, umi_editor_save_as */

/* If a search panel API is available, prefer to include it.
   Some branches place it under src/search/, others under src/.
   We try both include styles to maximize compatibility. */
#if __has_include("search_panel.h")
#  include "search_panel.h"
#elif __has_include("search/search_panel.h")
#  include "search/search_panel.h"
#endif

/* ------------------------------------------------------------------------- */
/* Helper to extract a UmiStatus* from the app (safe if ua or ua->status is NULL) */

static inline UmiStatus *umi_actions_status_or_null(UmiApp *ua) {
    UmiStatus *st = NULL;
    /* UmiApp is an opaque forward-declared struct in this header.
       We only *read* the status pointer field in a guarded way.
       The field name is 'status' in current sources. */
    struct _UmiApp { GtkWidget *win; GtkWidget *root; UmiEditor *ed; UmiStatusBar *status; UmiSearchPanel *search; };
    if (ua && ((struct _UmiApp*)ua)->status) {
        st = umi_status_bar_inner(((struct _UmiApp*)ua)->status);
    }
    return st;
}

/* Accessors for editor & search kept minimal and guarded. */
static inline UmiEditor *umi_actions_editor_or_null(UmiApp *ua) {
    struct _UmiApp { GtkWidget *win; GtkWidget *root; UmiEditor *ed; UmiStatusBar *status; UmiSearchPanel *search; };
    return ua ? ((struct _UmiApp*)ua)->ed : NULL;
}

static inline UmiSearchPanel *umi_actions_search_or_null(UmiApp *ua) {
    struct _UmiApp { GtkWidget *win; GtkWidget *root; UmiEditor *ed; UmiStatusBar *status; UmiSearchPanel *search; };
    return ua ? ((struct _UmiApp*)ua)->search : NULL;
}

/* ------------------------------------------------------------------------- */
/* Action: Open command palette (placeholder implementation) */

void umi_action_palette(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    UmiStatus *st = umi_actions_status_or_null(ua);
    if (st) umi_status_flash(st, "Command palette not implemented yet", 1200);
}

/* ------------------------------------------------------------------------- */
/* Action: Run (placeholder) */

void umi_action_run(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    UmiStatus *st = umi_actions_status_or_null(ua);
    if (st) umi_status_flash(st, "Run: not wired yet", 1200);
}

/* ------------------------------------------------------------------------- */
/* Action: Stop (placeholder) */

void umi_action_stop(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    UmiStatus *st = umi_actions_status_or_null(ua);
    if (st) umi_status_flash(st, "Stop: not wired yet", 1200);
}

/* ------------------------------------------------------------------------- */
/* Action: Focus Search input if available, otherwise flash a message */

void umi_action_focus_search(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    UmiSearchPanel *sp = umi_actions_search_or_null(ua);

    /* Prefer the dedicated API when present. Some branches expose
       umi_search_panel_focus_query(). If not detected at compile time,
       we fall back to a status flash (keeps build green). */
#if defined(umi_search_panel_focus_query) || defined(HAVE_UMI_SEARCH_PANEL_FOCUS_QUERY)
    if (sp) {
        umi_search_panel_focus_query(sp);
        return;
    }
#else
    (void)sp;
#endif

    UmiStatus *st = umi_actions_status_or_null(ua);
    if (st) umi_status_flash(st, "Focus: Search", 900);
}

/* ------------------------------------------------------------------------- */
/* Action: Save current editor buffer */

void umi_action_save(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    GError *err = NULL;

    UmiEditor *ed = umi_actions_editor_or_null(ua);
    if (! ed) return;

    if (! umi_editor_save(ed, &err) && err) {
        UmiStatus *st = umi_actions_status_or_null(ua);
        if (st) umi_status_flash(st, err->message ? err->message : "Save failed", 1600);
        g_clear_error(&err);
    } else {
        UmiStatus *st = umi_actions_status_or_null(ua);
        if (st) umi_status_flash(st, "Saved", 800);
    }
}

/* ------------------------------------------------------------------------- */
/* Action: Save As for current editor buffer */

void umi_action_save_as(GtkWidget *widget, gpointer user) {
    (void)widget;
    UmiApp *ua = (UmiApp*)user;
    GError *err = NULL;

    UmiEditor *ed = umi_actions_editor_or_null(ua);
    if (! ed) return;

    if (! umi_editor_save_as(ed, &err) && err) {
        UmiStatus *st = umi_actions_status_or_null(ua);
        if (st) umi_status_flash(st, err->message ? err->message : "Save As failed", 1600);
        g_clear_error(&err);
    } else {
        UmiStatus *st = umi_actions_status_or_null(ua);
        if (st) umi_status_flash(st, "Saved As", 800);
    }
}

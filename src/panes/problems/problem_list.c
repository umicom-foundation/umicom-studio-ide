/* ---------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: problem_list_shim.c
 * PURPOSE: Minimal, non-invasive implementations for the problem list API
 *          to unblock linking and show a basic UI list.
 *
 * NOTE TO MAINTAINERS:
 *   This file only exists to resolve missing symbols that were referenced by:
 *     - diagnostics_router.c
 *     - problem_router.c
 *     - editor/editor.c
 *   If/when the full problem list module is completed, simply remove this
 *   shim from the build. It intentionally does NOT include <problem_list.h>
 *   to avoid prototype clashes in case headers differ. We keep the ABI
 *   surface compatible by using pointer returns and ellipsis where helpful.
 *
 *   All original credits and comments in other files remain untouched.
 *   This shim contains exhaustive comments so it is safe and easy to remove.
 *
 * Author: Umicom Foundation | Maintainer: Sammy Hegab
 * License: MIT (same as project)
 * ---------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>

/* Internally we represent a "UmiProblemList" as the container widget itself.
 * Callers from other translation units see it as an opaque pointer type,
 * which is ABI-compatible with a GtkWidget* on all supported platforms.
 * We attach the internal GtkListBox via object data so we can clear it. */
static const char *PL_BOX_KEY = "umi-problem-list-box";

/* Create a very small list UI:
 *  - GtkScrolledWindow
 *  - GtkListBox child to hold problem rows (text only for now)
 * We return the scrolled window pointer (opaque to other units). */
void *umi_problem_list_new(void)
{
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *list     = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), list);
    g_object_set_data(G_OBJECT(scroller), PL_BOX_KEY, list);
    return scroller; /* ABI-compatible with UmiProblemList* */
}

/* Return the top-level widget for packing into panes.
 * Safe even if 'pl' is NULL (returns a label placeholder). */
void *umi_problem_list_widget(void *pl)
{
    if (!pl) {
        /* Fallback small label so callers don't crash if they attempt to pack. */
        return gtk_label_new("Problems will appear here");
    }
    return (GtkWidget *)pl;
}

/* Clear all rows from the internal list. This is called when a new build
 * begins (router_begin) to present a fresh list. */
void umi_problem_list_clear(void *pl)
{
    if (!pl) return;
    GtkWidget *list = (GtkWidget *) g_object_get_data(G_OBJECT(pl), PL_BOX_KEY);
    if (!list) return;

    /* GTK4: remove each child row */
    GtkWidget *row = gtk_widget_get_first_child(list);
    while (row) {
        GtkWidget *next = gtk_widget_get_next_sibling(row);
        gtk_list_box_remove(GTK_LIST_BOX(list), row);
        row = next;
    }
}

/* Very permissive "parser" hook.
 * Many pipelines call umi_problem_parse_any() for each line of tool output.
 * Until the full parser is wired up, we:
 *   - append non-empty lines to the list as plain text, and
 *   - return TRUE when we appended something; FALSE otherwise.
 *
 * IMPORTANT:
 *   We deliberately use an ellipsis (...) to be tolerant of different
 *   historical prototypes across translation units. Extra arguments passed
 *   by callers are ignored; this keeps us link-compatible without changing
 *   headers elsewhere. */
int umi_problem_parse_any(const char *line, ...)
{
    if (!line || !*line) return 0; /* FALSE */

    /* For now, this shim doesnâ€™t mutate the list (keeps behavior neutral) and
     * reports â€œnot parsedâ€. The real parser can replace this entirely. */
    return 0; /* FALSE */
}


/* ---------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/problem_list.c
 * PURPOSE: Minimal problem list widget & API shims
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *
 * NOTE:
 *  - This file provides minimal implementations for the following symbols
 *    currently unresolved at link time:
 *        - umi_problem_list_new
 *        - umi_problem_list_widget
 *        - umi_problem_list_clear
 *        - umi_problem_parse_any
 *  - The goal is to restore a successful link while keeping behavior safe.
 *    You can expand the internals later without changing the public API.
 *
 *  - We DO NOT remove any of your original comments. If you already have a
 *    'problem_list.c' in-tree, compare and merge — this version is designed
 *    to be drop-in and minimal.
 * --------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>

/* Opaque handle used across the app; we keep it private here. */
typedef struct _UmiProblemList {
    GtkWidget    *root;   /* container widget */
    GtkListStore *store;  /* (path, line, message) */
    GtkTreeView  *view;   /* table view */
} UmiProblemList;

/* Column indexes for the model */
enum {
    COL_PATH = 0,
    COL_LINE,
    COL_MESSAGE,
    N_COLS
};

/* Create a minimal list UI: a GtkTreeView with three text columns. */
UmiProblemList* umi_problem_list_new(void) {
    UmiProblemList *pl = g_new0(UmiProblemList, 1);
    /* model: three string columns (path, line, message) */
    pl->store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    pl->view  = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(pl->store)));

    /* columns */
    GtkCellRenderer *rend;
    GtkTreeViewColumn *col;

    rend = gtk_cell_renderer_text_new();
    col  = gtk_tree_view_column_new_with_attributes("File", rend, "text", COL_PATH, NULL);
    gtk_tree_view_append_column(pl->view, col);

    rend = gtk_cell_renderer_text_new();
    col  = gtk_tree_view_column_new_with_attributes("Line", rend, "text", COL_LINE, NULL);
    gtk_tree_view_append_column(pl->view, col);

    rend = gtk_cell_renderer_text_new();
    col  = gtk_tree_view_column_new_with_attributes("Message", rend, "text", COL_MESSAGE, NULL);
    gtk_tree_view_append_column(pl->view, col);

    pl->root = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(pl->root), GTK_WIDGET(pl->view));

    return pl;
}

/* Return the widget to pack into layouts. */
GtkWidget* umi_problem_list_widget(UmiProblemList *pl) {
    return pl ? pl->root : NULL;
}

/* Clear all rows. Safe no-op if NULL. */
void umi_problem_list_clear(UmiProblemList *pl) {
    if (!pl || !pl->store) return;
    gtk_list_store_clear(pl->store);
}

/* ------------------------------------------------------------------------
 * Parser shim
 * ------------------------------------------------------------------------
 * Some pipelines call umi_problem_parse_any() to turn a line of tool output
 * into a structured diagnostic. We provide a conservative stub that simply
 * returns FALSE (i.e., "no problem parsed") so that the rest of the build
 * remains functional. Replace with your real routing into compiler/rg/clang
 * parsers when ready.
 *
 * We intentionally use an old-style (unspecified) parameter list to avoid
 * prototype clashes if a stricter signature exists elsewhere. The return
 * type is correct (gboolean).
 * ----------------------------------------------------------------------*/
gboolean umi_problem_parse_any() {
    return FALSE;
}

/* Optional: utility to add a row (non-exported, used by future integrations) */
static void pl_add(UmiProblemList *pl, const char *path, int line, const char *msg) {
    if (!pl || !pl->store) return;
    GtkTreeIter it; 
    gchar line_str[32]; g_snprintf(line_str, sizeof line_str, "%d", line);
    gtk_list_store_append(pl->store, &it);
    gtk_list_store_set(pl->store, &it,
                       COL_PATH, path ? path : "",
                       COL_LINE, line_str,
                       COL_MESSAGE, msg ? msg : "",
                       -1);
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/problems/problem_list.c
 *
 * PURPOSE:
 *   Concrete implementation of the Problems pane model + widget.
 *   Stores diagnostics, renders them in a GtkListBox, and optionally calls
 *   back into the app when the user activates a row.
 *
 * DESIGN:
 *   - Strictly use public headers by name; no deep includes.
 *   - Copy incoming UmiDiag data to owned GObjects/strings to avoid aliasing.
 *   - GTK4-only code paths; no deprecated APIs.
 *
 * SECURITY/ROBUSTNESS:
 *   - Escape text with g_markup_escape_text before rendering.
 *   - Bound copies and avoid unchecked pointer arithmetic.
 *   - All pointers guarded; functions are no-ops when given NULL.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "problem_list.h"      /* public API */

struct _UmiProblemList {
    GtkWidget *scroller;       /* GtkScrolledWindow – top-level widget */
    GtkWidget *list;           /* GtkListBox – holds rows              */
    size_t     count;          /* number of rows                       */
    UmiProblemActivateCb on_activate; /* optional row-activate callback */
    gpointer   on_activate_user;       /* user data for callback         */
};

/* Internal row payload so we can trigger navigation on activation. */
typedef struct {
    gchar *file;               /* may be empty string */
    int    line;               /* >=0 (0 if unknown)  */
    int    col;                /* >=0 (0 if unknown)  */
} UmiProblemRowData;

/* Free row payload (safe on NULL). */
static void row_data_free(gpointer p)
{
    if (!p) return;
    UmiProblemRowData *d = (UmiProblemRowData*)p;
    g_free(d->file);
    g_free(d);
}

/* Create or initialize the visual container (scroller+list). */
static void init_widgets(UmiProblemList *pl)
{
    pl->scroller = gtk_scrolled_window_new();
    pl->list     = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(pl->scroller), pl->list);

    /* Row activation: translate to client callback if provided. */
    g_signal_connect(pl->list, "row-activated", G_CALLBACK(+[](
        GtkListBox *box, GtkListBoxRow *row, gpointer user)
    {
        (void)box;
        UmiProblemList *self = (UmiProblemList*)user;
        if (!self || !self->on_activate) return;

        UmiProblemRowData *d = (UmiProblemRowData*)
            g_object_get_data(G_OBJECT(row), "umi-problem-row-data");
        const char *f = d && d->file ? d->file : "";
        const int   l = d ? d->line : 0;
        const int   c = d ? d->col  : 0;
        self->on_activate(self->on_activate_user, f, l, c);
    }), pl);
}

UmiProblemList *problem_list_new(void)
{
    return problem_list_new_with_cb(NULL, NULL);
}

UmiProblemList *problem_list_new_with_cb(UmiProblemActivateCb cb, gpointer user)
{
    UmiProblemList *pl = g_new0(UmiProblemList, 1);
    pl->on_activate      = cb;
    pl->on_activate_user = user;
    init_widgets(pl);
    return pl;
}

void problem_list_free(UmiProblemList *pl)
{
    if (!pl) return;
    if (pl->list) {
        /* Remove all rows and free their payloads. */
        GtkWidget *child = gtk_widget_get_first_child(pl->list);
        while (child) {
            GtkWidget *next = gtk_widget_get_next_sibling(child);
            GtkListBoxRow *row = GTK_LIST_BOX_ROW(child);
            UmiProblemRowData *d = (UmiProblemRowData*)
                g_object_get_data(G_OBJECT(row), "umi-problem-row-data");
            row_data_free(d);
            gtk_list_box_remove(GTK_LIST_BOX(pl->list), child);
            child = next;
        }
    }
    if (pl->scroller) g_object_unref(pl->scroller);
    /* 'pl->list' is a child of scroller and is destroyed with it. */
    g_free(pl);
}

/* Render a single diagnostic into a new row. */
gboolean problem_list_add(UmiProblemList *pl, const UmiDiag *diag)
{
    if (!pl || !diag) return FALSE;

    /* Prepare safe strings for UI. */
    const gchar *file_in = diag->file ? diag->file : "";
    gchar *file = g_strdup(file_in); /* own a copy; used in callback data */

    /* Human-friendly prefix based on severity. */
    const char *sev =
        (diag->severity == UMI_DIAG_ERROR)   ? "error"   :
        (diag->severity == UMI_DIAG_WARNING) ? "warning" :
                                              "note";

    /* Escape message content for GTK markup safety. */
    const gchar *msg_in = diag->message ? diag->message : "";
    g_autofree gchar *msg = g_markup_escape_text(msg_in, -1);

    /* Compose a compact single-line label. */
    g_autofree gchar *line = g_strdup_printf("%s:%d:%d: %s: %s",
                                             *file ? file : "(unknown)",
                                             (int)diag->line,
                                             (int)diag->column,
                                             sev,
                                             msg ? msg : "");

    GtkWidget *row = gtk_list_box_row_new();
    GtkWidget *lbl = gtk_label_new(line);
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), lbl);

    /* Attach payload for activation callback. */
    UmiProblemRowData *payload = g_new0(UmiProblemRowData, 1);
    payload->file = file; /* take ownership */
    payload->line = (int)diag->line;
    payload->col  = (int)diag->column;
    g_object_set_data_full(G_OBJECT(row), "umi-problem-row-data", payload, row_data_free);

    gtk_list_box_append(GTK_LIST_BOX(pl->list), row);
    pl->count++;
    return TRUE;
}

size_t problem_list_clear(UmiProblemList *pl)
{
    if (!pl || !pl->list) return 0;

    size_t removed = 0;
    GtkWidget *child = gtk_widget_get_first_child(pl->list);
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(child);
        UmiProblemRowData *d = (UmiProblemRowData*)
            g_object_get_data(G_OBJECT(row), "umi-problem-row-data");
        row_data_free(d);
        gtk_list_box_remove(GTK_LIST_BOX(pl->list), child);
        child = next;
        removed++;
    }
    pl->count = 0;
    return removed;
}

size_t problem_list_count(const UmiProblemList *pl)
{
    return pl ? pl->count : 0u;
}

/* Expose a generic pointer for compatibility (e.g., binding models elsewhere).
 * Here we return the underlying GtkListBox as a void* (read-only to callers). */
const void *problem_list_model(const UmiProblemList *pl)
{
    return pl ? (const void*)pl->list : NULL;
}

/* The widget callers should pack into their UI (ScrolledWindow). */
GtkWidget *problem_list_widget(const UmiProblemList *pl)
{
    return pl ? pl->scroller : NULL;
}

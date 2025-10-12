/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/app_menu_llm.c
 * PURPOSE: Lightweight LLM menu strip for the main window (pure C, no XML).
 *          Self-contained: depends only on this module’s public headers.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------
 * Design notes (loose coupling):
 * - Public header is included from this module’s include/ directory.
 * - We DO NOT include other modules’ private headers.
 * - Cross-module hooks (e.g., "open LLM Lab", "Save") are handled via:
 *     * a forward-declared function (umi_llm_lab_new), and
 *     * a tiny heap ActionHolder that stores a function pointer safely.
 * - No casting between object pointers and function pointers (ISO C clean).
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>

/* Prefer the module-local public header path; fallback if your tree differs. */
#if __has_include("include/app_menu_llm.h")
#  include "include/app_menu_llm.h"
#else
#  include "app_menu_llm.h"
#endif

/* ────────────────────────────────────────────────────────────────────────── */
/* Optional: tolerate different locations of the LLM Lab widget header.
 * If none is found, we provide a forward declaration so the code still builds
 * (linking will succeed only if the symbol is available in your final link).
 */
#if __has_include("llm_lab.h")
#  include "llm_lab.h"
#elif __has_include("../llm_lab/include/llm_lab.h")
#  include "../llm_lab/include/llm_lab.h"
#elif __has_include("gui/llm_lab.h")
#  include "gui/llm_lab.h"
#else
/* Forward declare to avoid hard dependency at compile time. */
GtkWidget *umi_llm_lab_new(void);
#endif

/* ────────────────────────────────────────────────────────────────────────── */
/* Safe callback storage without function-pointer casts.
 *
 * We store a tiny heap struct (ActionHolder) under a GObject data key.
 * This avoids the “ISO C forbids conversion of function pointer to object
 * pointer” warning and keeps the module warning-clean under -Wpedantic.
 */
typedef void (*UmiSimpleAction)(gpointer user);
typedef struct { UmiSimpleAction cb; } ActionHolder;

#define UMI_DATA_CB_KEY "umi.cb"  /* where we store ActionHolder*        */
#define UMI_DATA_UD_KEY "umi.ud"  /* where we store user payload (gpointer) */

/* ────────────────────────────────────────────────────────────────────────── */
/* LLM Lab button handler:
 * Creates a simple transient window and embeds the LLM Lab widget if present.
 */
static void
on_click_llm_lab(GtkButton *btn, gpointer user_data)
{
    (void)user_data;

    /* Try to create the LLM Lab widget. If unavailable, show a fallback. */
    GtkWidget *lab = NULL;
#if defined(umi_llm_lab_new)
    lab = umi_llm_lab_new();
#endif
    if (!lab) {
        lab = gtk_label_new("LLM Lab module not linked");
    }

    GtkRoot   *root   = gtk_widget_get_root(GTK_WIDGET(btn));
    GtkWindow *parent = GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL;

    GtkWidget *win = gtk_window_new();                       /* GTK4 API */
    gtk_window_set_title(GTK_WINDOW(win), "LLM Lab");
    gtk_window_set_default_size(GTK_WINDOW(win), 720, 520);
    if (parent) gtk_window_set_transient_for(GTK_WINDOW(win), parent);
    gtk_window_set_modal(GTK_WINDOW(win), FALSE);
    gtk_window_set_child(GTK_WINDOW(win), lab);

    /* GTK4-friendly presentation (no deprecated gtk_widget_show). */
    gtk_window_present(GTK_WINDOW(win));
}

/* Save button handler:
 * Reads ActionHolder + payload from the *bar* (passed as user_data),
 * then invokes the stored callback if present.
 */
static void
on_click_save(GtkButton *btn, gpointer user_data)
{
    (void)btn;

    /* Retrieve the stored callback holder + user payload. */
    ActionHolder *got = g_object_get_data(G_OBJECT(user_data), UMI_DATA_CB_KEY);
    gpointer      ud  = g_object_get_data(G_OBJECT(user_data), UMI_DATA_UD_KEY);

    if (got && got->cb) {
        got->cb(ud);
    } else {
        g_message("Save action not available (no callback wired).");
    }
}

/* ────────────────────────────────────────────────────────────────────────── */
/* Public constructor:
 * Builds a very small “menu bar” row containing:
 *   [ LLM Lab ]                   [ Save ]
 * The Save button bridges back to the application via the provided callback.
 */
GtkWidget *
umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user_data)
{
    /* Container that acts like a minimal "menu bar" row. */
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(bar, "toolbar");

    /* --- LLM Lab button --------------------------------------------------- */
    GtkWidget *btn_lab = gtk_button_new_with_label("LLM Lab");
    g_signal_connect(btn_lab, "clicked", G_CALLBACK(on_click_llm_lab), NULL);
    gtk_box_append(GTK_BOX(bar), btn_lab);

    /* --- Flexible spacer to push Save to the right ------------------------ */
    GtkWidget *spacer = gtk_label_new(NULL);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(bar), spacer);

    /* --- Save button ------------------------------------------------------- */
    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    gtk_box_append(GTK_BOX(bar), btn_save);

    /* Store callback safely via a tiny heap holder (no func↔ptr casts). */
    ActionHolder *h = g_new(ActionHolder, 1);
    h->cb = on_save;

    /* The bar owns the holder’s lifetime; it will be freed automatically. */
    g_object_set_data_full(G_OBJECT(bar), UMI_DATA_CB_KEY, h, g_free);

    /* Store the user payload (we don’t own/free it). */
    g_object_set_data(G_OBJECT(bar), UMI_DATA_UD_KEY, user_data);

    /* Hook the Save button to read from the bar and invoke the callback. */
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_click_save), bar);

    return bar;
}
/*--- end of file ---*/
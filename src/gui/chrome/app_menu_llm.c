/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/app_menu_llm.c
 *
 * PURPOSE:
 *   Lightweight LLM menu strip for the main window (pure C, no Builder XML).
 *
 * DESIGN:
 *   - Header by name only (no deep paths).
 *   - Uses a tiny heap holder to store the Save callback safely (no func↔ptr casts).
 *   - Optional LLM Lab module via weak symbol; if missing, we show a fallback.
 *
 * SECURITY/ROBUSTNESS:
 *   - All pointers guarded; no unbounded string ops.
 *   - No reliance on non-portable designated struct padding.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "app_menu_llm.h"

/* Optional cross-module: declare weakly so we can test at runtime. */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) GtkWidget *umi_llm_lab_new(void);
#else
/* If weak isn't supported, we will attempt to call directly and rely on link-time presence. */
GtkWidget *umi_llm_lab_new(void); /* may be unresolved if not linked */
#endif

/* Callback holder (stored on the container object). */
typedef void (*UmiSimpleActionFn)(gpointer user);
typedef struct { UmiSimpleActionFn cb; gpointer user; } ActionHolder;

#define UMI_DATA_CB_KEY "umi.llm.cb"

static void on_click_llm_lab(GtkButton *btn, gpointer user_data)
{
    (void)user_data;

    GtkWidget *lab = NULL;
#if defined(__GNUC__) || defined(__clang__)
    if (umi_llm_lab_new) lab = umi_llm_lab_new();  /* only if symbol is linked */
#else
    lab = umi_llm_lab_new(); /* may or may not resolve depending on your link */
#endif
    if (!lab) lab = gtk_label_new("LLM Lab module not linked");

    GtkRoot   *root   = gtk_widget_get_root(GTK_WIDGET(btn));
    GtkWindow *parent = GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL;

    GtkWidget *win = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(win), "LLM Lab");
    gtk_window_set_default_size(GTK_WINDOW(win), 720, 520);
    if (parent) gtk_window_set_transient_for(GTK_WINDOW(win), parent);
    gtk_window_set_modal(GTK_WINDOW(win), FALSE);
    gtk_window_set_child(GTK_WINDOW(win), lab);
    gtk_window_present(GTK_WINDOW(win));  /* GTK4 */
}

static void on_click_save(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    ActionHolder *h = (ActionHolder*)g_object_get_data(G_OBJECT(user_data), UMI_DATA_CB_KEY);
    if (h && h->cb) h->cb(h->user);
    else            g_message("Save action not available (no callback wired).");
}

GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user)
{
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(bar, "toolbar");

    GtkWidget *btn_lab = gtk_button_new_with_label("LLM Lab");
    g_signal_connect(btn_lab, "clicked", G_CALLBACK(on_click_llm_lab), NULL);
    gtk_box_append(GTK_BOX(bar), btn_lab);

    GtkWidget *spacer = gtk_label_new(NULL);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(bar), spacer);

    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    gtk_box_append(GTK_BOX(bar), btn_save);

    ActionHolder *h = g_new0(ActionHolder, 1);
    h->cb = on_save;
    h->user = user;
    g_object_set_data_full(G_OBJECT(bar), UMI_DATA_CB_KEY, h, g_free);

    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_click_save), bar);
    return bar;
}

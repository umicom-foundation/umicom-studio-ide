/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app_menu_llm.c
 * PURPOSE: Lightweight menu strip that exposes LLM-related actions
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "app_menu_llm.h"

/* Be tolerant of different header placements across trees. */
#if __has_include("llm_lab.h")
#  include "llm_lab.h"
#elif __has_include("gui/llm_lab.h")
#  include "gui/llm_lab.h"
#elif __has_include("gui/llm_lab.h")
#  include "gui/llm_lab.h"
#else
/* Final fallback: forward declare so we can still build. */
GtkWidget *umi_llm_lab_new(void);
#endif

/* Open the LLM Lab in a simple transient window. */
static void on_click_llm_lab(GtkButton *btn, gpointer user_data) {
    (void)user_data;
    GtkWidget *lab = umi_llm_lab_new();
    if (!lab) return;

    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(btn));
    GtkWindow *parent = GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL;

    GtkWidget *win = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(win), "LLM Lab");
    gtk_window_set_default_size(GTK_WINDOW(win), 720, 520);
    if (parent) gtk_window_set_transient_for(GTK_WINDOW(win), parent);
    gtk_window_set_modal(GTK_WINDOW(win), FALSE);
    gtk_window_set_child(GTK_WINDOW(win), lab);
    gtk_widget_show(win);
}

/* Bridge the Save button to the app-provided callback. */
static void on_click_save(GtkButton *btn, gpointer user_data) {
    (void)btn;
    UmiSimpleAction cb = NULL;
    gpointer payload = NULL;

    /* We store the pair (cb, payload) into the button as data. */
    cb = (UmiSimpleAction)g_object_get_data(G_OBJECT(user_data), "umi.cb");
    payload = g_object_get_data(G_OBJECT(user_data), "umi.ud");

    if (cb) cb(payload);
}

/* Public constructor used by src/app.c */
GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user_data) {
    /* Container that acts like a minimal "menu bar" row. */
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(bar, "toolbar");

    /* LLM Lab button */
    GtkWidget *btn_lab = gtk_button_new_with_label("LLM Lab");
    g_signal_connect(btn_lab, "clicked", G_CALLBACK(on_click_llm_lab), NULL);
    gtk_box_append(GTK_BOX(bar), btn_lab);

    /* Spacer */
    GtkWidget *spacer = gtk_label_new(NULL);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(bar), spacer);

    /* Save button (hooks into the app callback) */
    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    gtk_box_append(GTK_BOX(bar), btn_save);

    /* We attach the callback pointer + user data to the bar and read it in on_click_save. */
    g_object_set_data(G_OBJECT(bar), "umi.cb", (gpointer)on_save);
    g_object_set_data(G_OBJECT(bar), "umi.ud", user_data);
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_click_save), bar);

    return bar;
}

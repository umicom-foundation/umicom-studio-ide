/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/status_bar.c
 *
 * PURPOSE:
 *   Minimal status bar (GTK4) with set() and flash(ms) helpers. No CSS used.
 *
 * DESIGN:
 *   - Root is a vertical box: [ GtkSeparator ] + [ horizontal content box ].
 *   - Spacing/margins in code; no gtk_widget_add_css_class calls.
 *
 * SECURITY/ROBUSTNESS:
 *   - All pointers guarded; flash timeout id cleared on cancel.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "status_bar.h"

struct _UmiStatusBar {
    GtkWidget *root;     /* vertical box: separator + content row              */
    GtkWidget *row;      /* horizontal row containing the text label           */
    GtkWidget *label;    /* shows the current status text                      */
    guint      flash_id; /* timeout source id for flash(); 0 when inactive     */
};

static gboolean flash_clear_cb(gpointer data)
{
    UmiStatusBar *sb = (UmiStatusBar *)data;
    if (!sb) return G_SOURCE_REMOVE;
    gtk_label_set_text(GTK_LABEL(sb->label), "");
    sb->flash_id = 0;
    return G_SOURCE_REMOVE;
}

UmiStatusBar *umi_status_bar_new(void)
{
    UmiStatusBar *sb = g_new0(UmiStatusBar, 1);

    sb->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(sb->root), sep);

    sb->row   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_margin_top   (sb->row, 4);
    gtk_widget_set_margin_bottom(sb->row, 4);
    gtk_widget_set_margin_start (sb->row, 8);
    gtk_widget_set_margin_end   (sb->row, 8);

    sb->label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(sb->label), 0.0f);
    gtk_widget_set_hexpand(sb->label, TRUE);

    gtk_box_append(GTK_BOX(sb->row), sb->label);
    gtk_box_append(GTK_BOX(sb->root), sb->row);

    return sb;
}

GtkWidget *umi_status_bar_widget(UmiStatusBar *sb)
{
    return sb ? sb->root : NULL;
}

void umi_status_bar_set(UmiStatusBar *sb, const char *text)
{
    if (!sb) return;
    if (sb->flash_id) { g_source_remove(sb->flash_id); sb->flash_id = 0; }
    gtk_label_set_text(GTK_LABEL(sb->label), text ? text : "");
}

void umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint ms)
{
    if (!sb) return;
    if (sb->flash_id) { g_source_remove(sb->flash_id); sb->flash_id = 0; }
    gtk_label_set_text(GTK_LABEL(sb->label), text ? text : "");
    if (ms == 0) return;
    sb->flash_id = g_timeout_add(ms, flash_clear_cb, sb);
}

void umi_status_bar_free(UmiStatusBar *sb)
{
    if (!sb) return;
    if (sb->flash_id) { g_source_remove(sb->flash_id); sb->flash_id = 0; }
    gtk_widget_destroy(sb->root);
    g_free(sb);
}

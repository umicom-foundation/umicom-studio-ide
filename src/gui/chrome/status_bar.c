/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/status_bar.c
 * PURPOSE: Minimal, self-contained status bar (pure C, no external deps).
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "status_bar.h"

struct _UmiStatusBar {
    GtkWidget *box;        /* container (exposed via umi_status_bar_widget) */
    GtkWidget *label;      /* label that shows the status text              */
    guint      flash_id;   /* timeout source id for flash()                 */
};

static gboolean
flash_clear_cb(gpointer data)
{
    UmiStatusBar *sb = (UmiStatusBar *)data;
    if (!sb) return G_SOURCE_REMOVE;
    gtk_label_set_text(GTK_LABEL(sb->label), "");
    sb->flash_id = 0;
    return G_SOURCE_REMOVE; /* one-shot */
}

UmiStatusBar *
umi_status_bar_new(void)
{
    UmiStatusBar *sb = g_new0(UmiStatusBar, 1);

    sb->box   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    sb->label = gtk_label_new("");

    gtk_widget_add_css_class(sb->box, "statusbar");
    gtk_widget_set_hexpand(sb->box, TRUE);
    gtk_label_set_xalign(GTK_LABEL(sb->label), 0.0f);

    gtk_box_append(GTK_BOX(sb->box), sb->label);
    return sb;
}

GtkWidget *
umi_status_bar_widget(UmiStatusBar *sb)
{
    return sb ? sb->box : NULL;
}

void
umi_status_bar_set(UmiStatusBar *sb, const char *text)
{
    if (!sb) return;
    if (sb->flash_id) { g_source_remove(sb->flash_id); sb->flash_id = 0; }
    gtk_label_set_text(GTK_LABEL(sb->label), text ? text : "");
}

void
umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint ms)
{
    if (!sb) return;
    if (sb->flash_id) { g_source_remove(sb->flash_id); sb->flash_id = 0; }
    gtk_label_set_text(GTK_LABEL(sb->label), text ? text : "");
    if (ms == 0) return;
    sb->flash_id = g_timeout_add(ms, flash_clear_cb, sb);
}
/*--- end of file ---*/
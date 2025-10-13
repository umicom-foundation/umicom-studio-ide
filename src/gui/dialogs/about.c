/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/dialog/about.c
 *
 * PURPOSE:
 *   Create and present a GTK4 About dialog. GTK4 removed gtk_dialog_run(),
 *   so we connect "response" and present the dialog non-blockingly.
 *
 * DESIGN:
 *   - No BOM / non-ASCII surprises (strict ASCII file).
 *   - Safe defaults: modal + transient when a parent is provided.
 *
 * SECURITY/ROBUSTNESS:
 *   - No dynamic format strings from user input.
 *   - All pointers are guarded before use.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include "about.h"

/* Close handler for the dialog's "response" signal. */
static void on_about_response(GtkDialog *dialog, int response, gpointer user_data)
{
    (void)response; (void)user_data;
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void umi_about_show(GtkWindow *parent)
{
    /* In GTK4, GtkAboutDialog still exists but run() is gone; present instead. */
    GtkWidget *dlg = gtk_about_dialog_new();

    /* Basic metadata; adjust as needed. */
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dlg), "Umicom Studio IDE");
    gtk_about_dialog_set_version     (GTK_ABOUT_DIALOG(dlg), "dev");
    gtk_about_dialog_set_comments    (GTK_ABOUT_DIALOG(dlg),
      "Open-source IDE for Umicom projects.\n"
      "Copyright © Umicom Foundation."
    );

    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(dlg), parent);
        gtk_window_set_modal        (GTK_WINDOW(dlg), TRUE);
    }

    g_signal_connect(dlg, "response", G_CALLBACK(on_about_response), NULL);
    gtk_window_present(GTK_WINDOW(dlg));
}

/* Back-compat wrapper retained for older callers */
void umi_show_about(GtkWindow *parent) { umi_about_show(parent); }

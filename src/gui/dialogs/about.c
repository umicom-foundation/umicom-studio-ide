/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/about.c
 * PURPOSE: About dialog (GTK4) â€” non-blocking, modal presentation
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>

/* Public helpers â€” both names provided to match existing callers */
void umi_about_show(GtkWindow *parent);
void umi_show_about(GtkWindow *parent);

/* Internal: close handler for the dialog's "response" signal */
static void on_about_response(GtkDialog *dialog, int response, gpointer user_data) {
    (void)response; (void)user_data;
    gtk_window_destroy(GTK_WINDOW(dialog));
}


void umi_about_show(GtkWindow *parent)
{
  /* In GTK4, gtk_dialog_run() was removed.
   * We build a GtkAboutDialog, set it modal/transient, connect "response",
   * and present it non-blockingly.
   */
  GtkWidget *dlg = gtk_about_dialog_new();

  /* Basic metadata (safe placeholders; customize as needed) */
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dlg), "Umicom Studio IDE");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dlg), "dev");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dlg),
    "An experimental, friendly IDE for Umicom projects.\n"
    "Copyright Â© Umicom Foundation."
  );

  if (parent) {
    gtk_window_set_transient_for(GTK_WINDOW(dlg), parent);
    gtk_window_set_modal(GTK_WINDOW(dlg), TRUE);
  }

  /* Close when the built-in button is clicked */
  g_signal_connect(dlg, "response", G_CALLBACK(on_about_response), NULL);

  /* Show it */
  gtk_window_present(GTK_WINDOW(dlg));
}

void umi_show_about(GtkWindow *parent)
{
  /* Back-compat wrapper: some modules may still call umi_show_about() */
  umi_about_show(parent);
}
/*---------------------------------------------------------------------------*/
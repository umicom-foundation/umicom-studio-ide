/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/about.c
 * PURPOSE: Show a simple about dialog
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "about.h"

void umi_about_show(GtkWindow *parent){
  GtkWidget *d = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(d), "Umicom Studio IDE");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(d),
    "Umicom Studio IDE — lightweight, hackable editor for the Umicom ecosystem.\n"
    "© 2025 Umicom Foundation. MIT License.");
  gtk_window_set_transient_for(GTK_WINDOW(d), parent);
  gtk_dialog_run(GTK_DIALOG(d));
  gtk_window_destroy(GTK_WINDOW(d));
}

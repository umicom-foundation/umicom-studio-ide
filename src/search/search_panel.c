/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/search_panel.c
 * PURPOSE: Minimal Search Panel wiring (placeholder for full UI)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>
#include "search_panel.h"

/*---------------------------------------------------------------------------
 * on_clicked:
 *   Temporary stub callback used during early UI wiring. Marked G_GNUC_UNUSED
 *   to avoid -Wunused-function warnings until connected in the .ui file.
 *---------------------------------------------------------------------------*/
static void on_clicked(void) G_GNUC_UNUSED;
static void on_clicked(void) {
  g_message("SearchPanel: click handler stub");
}

/*---------------------------------------------------------------------------
 * umi_search_panel_new:
 *   Create a trivial panel instance that wraps a plain GtkBox for now.
 *---------------------------------------------------------------------------*/
UmiSearchPanel *umi_search_panel_new(void) {
  UmiSearchPanel *sp = g_new0(UmiSearchPanel, 1);
  sp->widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  return sp;
}

/*---------------------------------------------------------------------------
 * umi_search_panel_widget:
 *   Accessor for the root widget of the panel.
 *---------------------------------------------------------------------------*/
GtkWidget *umi_search_panel_widget(UmiSearchPanel *sp) {
  g_return_val_if_fail(sp != NULL, NULL);
  return sp->widget;
}

/*---------------------------------------------------------------------------
 * umi_search_panel_run_example:
 *   Placeholder demonstrating how the panel might kick off a search.
 *   Currently just logs to the console to prove basic wiring works.
 *---------------------------------------------------------------------------*/
void umi_search_panel_run_example(UmiSearchPanel *sp) {
  g_return_if_fail(sp != NULL);
  g_message("SearchPanel: run_example() called (stub)");
}
/*---------------------------------------------------------------------------*/
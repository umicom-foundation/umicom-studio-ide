/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search_panel.h
 * PURPOSE: Simple search panel using the ripgrep runner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SEARCH_PANEL_H
#define UMICOM_SEARCH_PANEL_H

#include <gtk/gtk.h>
#include "rg_runner.h"
#include "../../util/log/include/status_util.h"

/* The Search Panel is a small widget with:
 * - An entry for the pattern the contributor wants to find.
 * - A 'Search' button that spawns ripgrep.
 * - A listbox that fills with 'file:line:col:text' matches.
 * The code is deliberately simple and heavily commented so a newcomer can follow. */
typedef struct {
  GtkWidget *root;
  GtkEntry  *entry;
  GtkButton *btn;
  GtkListBox *list;
  UmiStatus *status;
} UmiSearchPanel;

UmiSearchPanel *umi_search_panel_new(UmiStatus *status);

#endif /* UMICOM_SEARCH_PANEL_H */

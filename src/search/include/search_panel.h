/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/include/search_panel.h
 * PURPOSE: Public API for the Search Panel widget (GTK)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once

#include <gtk/gtk.h>
#include "rg_runner.h"
#include "rg_discovery.h"
#include "ripgrep_args.h"
#include "status_util.h"  /* from src/util/log/include */

/*---------------------------------------------------------------------------
 * UmiSearchPanel:
 *   Opaque type representing the panel state. For now this is a trivial
 *   wrapper around a GtkWidget, but it may evolve to keep settings/state.
 *---------------------------------------------------------------------------*/
typedef struct UmiSearchPanel {
  GtkWidget *widget;  /* owned by the panel; packed into the main UI */
} UmiSearchPanel;

/* Construct a new panel instance. Ownership: caller must later destroy it. */
UmiSearchPanel *umi_search_panel_new(void);

/* Return the root widget to pack into UI containers. Non-owning pointer. */
GtkWidget *umi_search_panel_widget(UmiSearchPanel *sp);

/* Execute a sample search (placeholder). Will emit results to the UI when
 * fully implemented. */
void umi_search_panel_run_example(UmiSearchPanel *sp);
/*---------------------------------------------------------------------------*/
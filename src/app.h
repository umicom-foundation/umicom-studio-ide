/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Project-wide small fixes to resolve current build errors
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_H
#define UMI_APP_H

#include <gtk/gtk.h>
#include "editor.h"
#include "search_panel.h"
#include "status_util.h"

/* Top-level app state */
typedef struct _UmiApp {
  GtkApplication *app;
  GtkWindow      *win;
  UmiEditor      *ed;
  /* NOTE: Use UmiStatus (status_util.h), not UmiStatusBar.
   * This fixes mismatched types when passing to helpers like umi_status_flash(). */
  UmiStatus      *status;
  UmiSearchPanel *search;
} UmiApp;

#endif /* UMI_APP_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app.h
 * PURPOSE: Application-level types and helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once

#include <gtk/gtk.h>
#include <glib.h>

#include "editor.h"
#include "search_panel.h"
#include "status_bar.h"     /* UmiStatusBar, umi_status_bar_* */

/* Forward declarations */
typedef struct _UmiApp UmiApp;

struct _UmiApp {
  GtkApplication *app;
  GtkWindow      *win;
  UmiEditor      *ed;
  UmiStatusBar   *status;   /* NOTE: This is a UmiStatusBar*, not UmiStatus*. */
  UmiSearchPanel *search;
};

/* Construct the full UI and return the top-level widget. */
GtkWidget *umi_app_build(UmiApp *ua);

/* Utility to open a file */
void umi_app_open_path(UmiApp *ua, const char *path);

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app.h
 * PURPOSE: Top-level GTK application wiring editor, tree, panels, and UI
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APP_H
#define UMICOM_APP_H

#include <gtk/gtk.h>
#include "editor.h"
#include "file_tree.h"
#include "search_panel.h"
#include "status_bar.h"

typedef struct _UmiApp {
  GtkApplication *app;
  GtkWindow *win;
  UmiEditor *ed;
  UmiFileTree *tree;
  UmiSearchPanel *search;
  UmiStatusBar *status;
} UmiApp;

GtkApplication *umi_app_new(void);
UmiApp        *umi_app_handle(GtkApplication *app);
GtkWindow     *umi_app_window(UmiApp *ua);
UmiEditor     *umi_app_editor(UmiApp *ua);

#endif /* UMICOM_APP_H */

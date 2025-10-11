/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: include/app.h
 * PURPOSE: Minimal public definition of UmiApp used by app_actions and UI modules.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_H
#define UMI_APP_H

#include <gtk/gtk.h>
struct UmiEditor;
struct UmiSearchPanel;
struct UmiStatus;
typedef struct UmiApp {
  GtkApplication *app;
  GtkWindow      *win;
  struct UmiEditor      *ed;
  struct UmiStatus      *status;
  struct UmiSearchPanel *search;
} UmiApp;
#endif /* UMI_APP_H */

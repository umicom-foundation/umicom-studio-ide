/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.h
 * PURPOSE: Declarations for high-level application keymap wiring.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_ACTIONS_H
#define UMI_APP_ACTIONS_H
#include <glib.h>
#include <gtk/gtk.h>
typedef struct UmiApp UmiApp;
typedef void (*UmiActionFn)(gpointer user);
typedef struct UmiKeymapCallbacks {
  UmiActionFn palette;
  UmiActionFn save;
  UmiActionFn save_as;
  UmiActionFn run;
  UmiActionFn stop;
  UmiActionFn focus_search;
} UmiKeymapCallbacks;
void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);
#endif /* UMI_APP_ACTIONS_H */

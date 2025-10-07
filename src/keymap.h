/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.h
 * PURPOSE: Simple keyboard shortcut installation (GTK4 EventControllerKey)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KEYMAP_H
#define UMICOM_KEYMAP_H

#include <gtk/gtk.h>

typedef void (*UmiActionFn)(gpointer user);

typedef struct {
  gpointer user;
  UmiActionFn palette;
  UmiActionFn save;
  UmiActionFn save_as;
  UmiActionFn run;
  UmiActionFn stop;
  UmiActionFn focus_search;
} UmiKeymapCallbacks;

/* Install a minimal keymap on a window using GtkEventControllerKey.
   Default bindings (Windows/Linux style):
     Ctrl+Shift+P : palette
     Ctrl+S       : save
     Ctrl+Shift+S : save_as
     F5           : run
     Shift+F5     : stop
     Ctrl+F       : focus_search
*/
void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km);

#endif /* UMICOM_KEYMAP_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.c
 * PURPOSE: Default accelerators mapping to actions
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "keymap.h"

void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *cb){
  (void)cb;
  if(!win) return;
  GtkApplication *app = GTK_APPLICATION(gtk_window_get_application(win));
  if(!app) return;

  /* Accelerators bound to actions defined in actions.c */
  gtk_application_set_accels_for_action(app, "app.open-folder", (const char*[]){"<Primary>o", NULL});
  gtk_application_set_accels_for_action(app, "app.save",        (const char*[]){"<Primary>s", NULL});
  gtk_application_set_accels_for_action(app, "app.run",         (const char*[]){"F5", NULL});
  gtk_application_set_accels_for_action(app, "app.stop",        (const char*[]){"Shift+F5", NULL});
  gtk_application_set_accels_for_action(app, "app.palette",     (const char*[]){"<Primary>p", NULL});
  gtk_application_set_accels_for_action(app, "app.preferences", (const char*[]){"<Primary>,", NULL});
  gtk_application_set_accels_for_action(app, "app.quit",        (const char*[]){"<Primary>q", NULL});
}

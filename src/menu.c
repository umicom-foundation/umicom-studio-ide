/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/menu.c
 * PURPOSE: Implementation of a minimal application menu
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "menu.h"

void umi_menu_install(GtkApplication *app){
  if(!app) return;
  GMenu *m = g_menu_new();
  GMenu *file = g_menu_new();
  g_menu_append(file, "Open Folder…", "app.open-folder");
  g_menu_append(file, "Save",         "app.save");
  g_menu_append(file, "Preferences",  "app.preferences");
  g_menu_append(file, "Quit",         "app.quit");
  g_menu_append_section(m, "File", G_MENU_MODEL(file)); g_object_unref(file);

  GMenu *run = g_menu_new();
  g_menu_append(run, "Run",  "app.run");
  g_menu_append(run, "Stop", "app.stop");
  g_menu_append_section(m, "Run", G_MENU_MODEL(run)); g_object_unref(run);

  GMenu *help = g_menu_new();
  g_menu_append(help, "Command Palette…", "app.palette");
  g_menu_append_section(m, "Tools", G_MENU_MODEL(help)); g_object_unref(help);

  gtk_application_set_menubar(app, G_MENU_MODEL(m));
  g_object_unref(m);
}

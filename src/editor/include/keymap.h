/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/include/keymap.h
 * PURPOSE:
 *   Centralized keybinding table + callbacks for window-scoped actions.
 *
 * DESIGN:
 *   - Uses GTK4/GAction (no deprecated APIs).
 *   - Lightweight C API: caller supplies callbacks and a user pointer.
 *
 * API:
 *   typedef struct UmiKeymapCallbacks { ... } UmiKeymapCallbacks;
 *   void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT 
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KEYMAP_H
#define UMICOM_KEYMAP_H

#include <gtk/gtk.h>

typedef struct UmiKeymapCallbacks {
  gpointer user;
  void (*palette)(gpointer user);
  void (*save)(gpointer user);
  void (*save_as)(gpointer user);
  void (*run)(gpointer user);
  void (*stop)(gpointer user);
  void (*focus_search)(gpointer user);
} UmiKeymapCallbacks;

/* Install a GtkShortcutController on the window and wire to callbacks. */
void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km);

#endif /* UMICOM_KEYMAP_H */
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.h
 * PURPOSE: Centralized keybinding table + callbacks
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KEYMAP_H
#define UMICOM_KEYMAP_H

#include <gtk/gtk.h>

/* Callbacks used by the keymap. Keep names stable to avoid churn. */
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

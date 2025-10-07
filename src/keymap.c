/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.c
 * PURPOSE: Centralized keybinding table + callbacks (implementation)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#include "keymap.h"

static void bind_key(GtkWindow *win, const char *accel, void (*fn)(gpointer), gpointer user)
{
  if (!win || !accel || !*accel || !fn) return;
  GtkShortcutController *ctl = GTK_SHORTCUT_CONTROLLER(gtk_shortcut_controller_new());
  gtk_shortcut_controller_set_scope(ctl, GTK_SHORTCUT_SCOPE_GLOBAL);
  GdkKeyval key = 0;
  GdkModifierType mods = 0;
  if (gtk_accelerator_parse(accel, &key, &mods))
  {
    GtkShortcut *sc = gtk_shortcut_new(
        gtk_shortcut_trigger_new_for_gdk_key(key, mods),
        gtk_callback_action_new((GtkShortcutFunc)fn, user, NULL));
    gtk_shortcut_controller_add_shortcut(ctl, sc);
    gtk_widget_add_controller(GTK_WIDGET(win), GTK_EVENT_CONTROLLER(ctl));
  }
}

void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km)
{
  if (!win || !km) return;
  bind_key(win, "<Control>s", km->save,        km->user);
  bind_key(win, "<Control><Shift>s", km->save_as, km->user);
  bind_key(win, "F1", km->palette,   km->user);
  bind_key(win, "F5", km->run,       km->user);
  bind_key(win, "<Shift>F5", km->stop, km->user);
  bind_key(win, "<Control>f", km->focus_search, km->user);
}

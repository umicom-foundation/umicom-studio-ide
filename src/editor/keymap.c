/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.c
 * PURPOSE: Centralized keybinding table + callbacks (GTK4, action-based)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "keymap.h"

typedef struct {
  void    (*fn)(gpointer user);
  gpointer user;
} UmiKMClosure;

static void on_simple_action(GSimpleAction *action, GVariant *param, gpointer data){
  (void)action; (void)param;
  UmiKMClosure *c = (UmiKMClosure*)data;
  if (c && c->fn) c->fn(c->user);
}

static void install_action(GtkWindow *win, const char *name, const char *accel,
                           void (*fn)(gpointer), gpointer user)
{
  if (!win || !name || !*name || !accel || !*accel || !fn) return;

  /* Create window-scoped simple action */
  /* Note: store closure on the action and free with g_object_set_data_full to avoid closure signature mismatch warnings. */
  GSimpleAction *act = g_simple_action_new(name, NULL);
  UmiKMClosure *c = g_new0(UmiKMClosure, 1);
  c->fn = fn;
  c->user = user;
  g_signal_connect_data(act, "activate", G_CALLBACK(on_simple_action),
                        c, (GClosureNotify)NULL, 0);
  g_object_set_data_full(G_OBJECT(act), "umi-km-closure", c, g_free);
  g_action_map_add_action(G_ACTION_MAP(win), G_ACTION(act));
  g_object_unref(act);

  /* Bind accelerator to the window action */
  GtkApplication *app = gtk_window_get_application(win);
  if (app){
    char detailed[128];
    g_snprintf(detailed, sizeof(detailed), "win.%s", name);
    const char *accels[] = { accel, NULL };
    gtk_application_set_accels_for_action(app, detailed, accels);
  }
}

void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km)
{
  if (!win || !km) return;
  /* Use stable, namespaced action names */
  install_action(win, "umi-palette",       "F1",                 km->palette,       km->user);
  install_action(win, "umi-save",          "<Control>s",         km->save,          km->user);
  install_action(win, "umi-save-as",       "<Control><Shift>s",  km->save_as,       km->user);
  install_action(win, "umi-run",           "F5",                 km->run,           km->user);
  install_action(win, "umi-stop",          "<Shift>F5",          km->stop,          km->user);
  install_action(win, "umi-focus-search",  "<Control>f",         km->focus_search,  km->user);
}

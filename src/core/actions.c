/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/actions.c
 * PURPOSE: Implementation of default actions (no-op callbacks)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/actions.h"

static void noop(GSimpleAction *a, GVariant *p, gpointer u){ (void)a;(void)p;(void)u; }

void umi_actions_install(GtkApplication *app){
  if(!app) return;
  const char *names[] = {
    "open-folder", "save", "run", "stop", "palette", "preferences", "quit", NULL
  };
  for(const char **n=names; *n; ++n){
    GSimpleAction *act = g_simple_action_new(*n, NULL);
    g_signal_connect(act, "activate", G_CALLBACK(noop), NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act));
  }
}

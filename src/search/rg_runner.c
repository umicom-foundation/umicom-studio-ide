/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/rg_runner.c
 * PURPOSE: Implementation of ripgrep spawner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/rg_runner.h"

GSubprocess *umi_rg_spawn(const UmiRgOpts *o, GError **err){
  if(!o || !o->pattern || !o->root) return NULL;
  GPtrArray *argv = g_ptr_array_new_with_free_func(g_free);
  g_ptr_array_add(argv, g_strdup("rg"));
  if(o->ignore_case) g_ptr_array_add(argv, g_strdup("-i"));
  g_ptr_array_add(argv, g_strdup("--vimgrep"));
  g_ptr_array_add(argv, g_strdup(o->pattern));
  g_ptr_array_add(argv, g_strdup(o->root));
  g_ptr_array_add(argv, NULL);

  GSubprocessLauncher *L = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE);
  GSubprocess *p = g_subprocess_launcher_spawnv(L, (const gchar * const*)argv->pdata, err);
  g_object_unref(L);
  g_ptr_array_free(argv, TRUE);
  return p;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/win_launcher.c
 * PURPOSE: Implementation of Windows/MSYS launcher
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "win_launcher.h"

GSubprocess* umi_win_spawn_with_msys_env(const char *cwd, char * const *argv, GError **err){
  GSubprocessLauncher *L = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE);
#ifdef G_OS_WIN32
  gchar *hint = umi_msys_path_hint();
  if(hint){
    const gchar *old = g_getenv("PATH");
    gchar *merged = g_strdup_printf("%s;%s", hint, old?old:"");
    g_subprocess_launcher_setenv(L, "PATH", merged, TRUE);
    g_free(merged);
    g_free(hint);
  }
#endif
  if(cwd) g_subprocess_launcher_set_cwd(L, cwd);
  GSubprocess *p = g_subprocess_launcher_spawnv(L, (const gchar * const*)argv, err);
  g_object_unref(L);
  return p;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/msys_env.c
 * PURPOSE: Implementation: query common env vars to build PATH hint
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "msys_env.h"

gchar *umi_msys_path_hint(void){
#ifdef G_OS_WIN32
  const char *msys = g_getenv("MSYS2_PATH_TYPE");
  const char *ucrt = g_getenv("MSYSTEM");
  const char *root = g_getenv("MSYS2_ROOT");
  if(ucrt || root){
    GString *s = g_string_new("");
    const char *bases[] = {
      root?root:"C:\\\\msys64",
      "C:\\\\msys64",
      NULL
    };
    for(int i=0;bases[i];++i){
      gchar *bin = g_build_filename(bases[i], "usr", "bin", NULL);
      g_string_append_printf(s, "%s;", bin);
      g_free(bin);
      bin = g_build_filename(bases[i], "mingw64", "bin", NULL);
      g_string_append_printf(s, "%s;", bin);
      g_free(bin);
    }
    return g_string_free(s, FALSE);
  }
#endif
  return NULL;
}

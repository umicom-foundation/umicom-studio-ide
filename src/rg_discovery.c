/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/rg_discovery.c
 * PURPOSE: Implementation of ripgrep discovery
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "rg_discovery.h"

static gboolean is_exe(const char *path){
  return path && *path && g_file_test(path, G_FILE_TEST_IS_EXECUTABLE);
}

gchar *umi_rg_find_binary(void){
  const char *env1 = g_getenv("USTUDIO_RG");
  if(is_exe(env1)) return g_strdup(env1);
  const char *env2 = g_getenv("UMI_RG");
  if(is_exe(env2)) return g_strdup(env2);
#ifdef G_OS_WIN32
  const char *cand = "rg.exe";
#else
  const char *cand = "rg";
#endif
  gchar *full = g_find_program_in_path(cand);
  return full; /* may be NULL */
}

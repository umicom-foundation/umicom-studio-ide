/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ripgrep_args.c
 * PURPOSE: Implementation of argv builder for ripgrep
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "ripgrep_args.h"

GPtrArray *umi_rg_build_argv(const UmiRgCfg *cfg){
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
#ifdef G_OS_WIN32
  g_ptr_array_add(a, g_strdup("rg.exe"));
#else
  g_ptr_array_add(a, g_strdup("rg"));
#endif
  g_ptr_array_add(a, g_strdup("--vimgrep"));
  if(!cfg->match_case) g_ptr_array_add(a, g_strdup("--ignore-case"));
  if(!cfg->regex) g_ptr_array_add(a, g_strdup("--fixed-strings"));
  g_ptr_array_add(a, g_strdup(cfg->query?cfg->query:""));
  g_ptr_array_add(a, g_strdup(cfg->folder?cfg->folder:"."));
  g_ptr_array_add(a, NULL);
  return a;
}

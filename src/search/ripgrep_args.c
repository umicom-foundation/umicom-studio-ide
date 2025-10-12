/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/ripgrep_args.c
 * PURPOSE: Build argv lists for invoking ripgrep
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <string.h>
#include "ripgrep_args.h"

/*---------------------------------------------------------------------------
 * umi_rg_args_make_simple:
 *   Construct:  [ "rg", "--hidden", "--line-number", "--vimgrep",
 *                 pattern, path, NULL ]
 *   This is a beginner-friendly preset that searches hidden files and
 *   produces vimgrep-compatible output (<file>:<line>:<col>:<match>).
 *---------------------------------------------------------------------------*/
char **umi_rg_args_make_simple(const char *pattern, const char *path) {
  g_return_val_if_fail(pattern != NULL, NULL);
  g_return_val_if_fail(path    != NULL, NULL);

  char **argvv = g_new0(char*, 7);
  argvv[0] = g_strdup("rg");
  argvv[1] = g_strdup("--hidden");
  argvv[2] = g_strdup("--line-number");
  argvv[3] = g_strdup("--vimgrep");
  argvv[4] = g_strdup(pattern);
  argvv[5] = g_strdup(path);
  argvv[6] = NULL;
  return argvv;
}

/*---------------------------------------------------------------------------
 * umi_rg_args_free:
 *   Release argv array allocated by umi_rg_args_make_simple().
 *---------------------------------------------------------------------------*/
void umi_rg_args_free(char **argvv) {
  if (!argvv) return;
  for (guint i = 0; argvv[i]; ++i) g_free(argvv[i]);
  g_free(argvv);
}
/*---------------------------------------------------------------------------*/
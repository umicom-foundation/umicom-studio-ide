/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/search_panel.c
 * PURPOSE: Minimal Search Panel wiring (placeholder for full UI)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/* NOTE [2025-10-14]:
 * This translation unit was previously a copy of the Search Panel code,
 * which caused duplicate exported symbols with src/search/search_panel.c.
 * We've kept the head comment intact (per project policy), and replaced the
 * implementation below with the proper ripgrep argv builder matching
 * src/search/include/ripgrep_args.h.
 */

#include <glib.h>
#include <string.h>
#include "ripgrep_args.h"

/*-----------------------------------------------------------------------------
 * umi_rg_args_make_simple:
 *   Build a NULL-terminated argv suitable for glib's g_spawn* APIs:
 *
 *     rg -n --no-heading --color=never <pattern> <path>
 *
 *   Ownership:
 *     - Returns a newly-allocated, NULL-terminated array of newly-allocated
 *       strings (g_strdup). Free with umi_rg_args_free() (g_strfreev).
 *   Notes:
 *     - If pattern is NULL/empty, ripgrep will error; we still return argv
 *       and let the caller capture stderr (keeps plumbing simple).
 *     - If path is NULL/empty, we default to ".".
 *---------------------------------------------------------------------------*/
char **umi_rg_args_make_simple(const char *pattern, const char *path)
{
  const char *safe_pattern = pattern ? pattern : "";
  const char *safe_path    = (path && *path) ? path : ".";

  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  g_ptr_array_add(a, g_strdup("rg"));
  g_ptr_array_add(a, g_strdup("-n"));              /* print line numbers */
  g_ptr_array_add(a, g_strdup("--no-heading"));    /* no file headers */
  g_ptr_array_add(a, g_strdup("--color=never"));   /* machine-friendly */
  g_ptr_array_add(a, g_strdup(safe_pattern));
  g_ptr_array_add(a, g_strdup(safe_path));
  g_ptr_array_add(a, NULL);

  /* Transfer the underlying char**; caller must free with g_strfreev. */
  return (char**)g_ptr_array_free(a, FALSE);
}

/*-----------------------------------------------------------------------------
 * umi_rg_args_free:
 *   Frees an argv vector previously created by umi_rg_args_make_simple().
 *   Safe with NULL.
 *---------------------------------------------------------------------------*/
void umi_rg_args_free(char **argvv)
{
  if (argvv) g_strfreev(argvv);
}
/*---------------------------------------------------------------------------*/

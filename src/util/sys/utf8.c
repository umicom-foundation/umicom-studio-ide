/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/utf8.c
 * PURPOSE: Implementation using GLib casefolding
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/utf8.h"

gboolean umi_utf8_icontains(const char *h, const char *n){
  if(!h || !n || !*n) return FALSE;
  gchar *H = g_utf8_casefold(h, -1);
  gchar *N = g_utf8_casefold(n, -1);
  gboolean ok = (g_strstr_len(H, -1, N) != NULL);
  g_free(H); g_free(N);
  return ok;
}

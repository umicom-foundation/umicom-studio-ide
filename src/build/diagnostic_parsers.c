/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/diagnostic_parsers.c
 * PURPOSE: Implement simple line parsers without regex
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <diagnostic_parsers.h>

static UmiDiagSeverity sev_from_token(const char *t){
  if(!t) return UMI_DIAG_ERROR;
  if(g_ascii_strncasecmp(t, "error", 5)==0)   return UMI_DIAG_ERROR;
  if(g_ascii_strncasecmp(t, "warning", 7)==0) return UMI_DIAG_WARNING;
  if(g_ascii_strncasecmp(t, "note", 4)==0)    return UMI_DIAG_NOTE;
  return UMI_DIAG_ERROR;
}

gboolean umi_diag_parse_gcc_clang(const char *s, UmiDiagParseResult *out){
  if(!s || !out) return FALSE;
  const char *a = strchr(s, ':');
  if(!a) return FALSE;
  const char *b = strchr(a+1, ':');
  if(!b) return FALSE;
  const char *c = strchr(b+1, ':');
  if(!c) return FALSE;

  out->file = g_strndup(s, a - s);
  out->line = (int)g_ascii_strtoll(a+1, NULL, 10);
  out->col  = (int)g_ascii_strtoll(b+1, NULL, 10);

  /* severity token up to next ':' */
  const char *d = strchr(c+1, ':');
  if(!d){ g_free(out->file); return FALSE; }
  gchar *sev = g_strndup(c+1, d-(c+1));
  out->sev = sev_from_token(sev);
  g_free(sev);

  out->msg = g_strdup(d+1);
  return TRUE;
}

gboolean umi_diag_parse_msvc(const char *s, UmiDiagParseResult *out){
  if(!s || !out) return FALSE;
  /* Expect: file(line[,col]): <sev> ... : message */
  const char *lpar = strchr(s, '(');
  const char *rpar = lpar ? strchr(lpar+1, ')') : NULL;
  const char *colon = rpar ? strchr(rpar, ':') : NULL;
  if(!lpar || !rpar || !colon) return FALSE;
  out->file = g_strndup(s, lpar - s);
  out->line = (int)g_ascii_strtoll(lpar+1, NULL, 10);
  /* Optional ,col */
  const char *comma = strchr(lpar+1, ',');
  out->col = (comma && comma < rpar) ? (int)g_ascii_strtoll(comma+1, NULL, 10) : 1;

  /* severity token after "): " up to next space or colon */
  const char *sev_beg = colon + 1;
  while(*sev_beg==' ') sev_beg++;
  const char *sev_end = sev_beg;
  while(*sev_end && *sev_end!=':' && *sev_end!=' ') sev_end++;
  gchar *sev = g_strndup(sev_beg, sev_end - sev_beg);
  out->sev = sev_from_token(sev);
  g_free(sev);

  const char *msg = strchr(sev_end, ':');
  out->msg = g_strdup(msg ? msg+1 : sev_end);
  return TRUE;
}

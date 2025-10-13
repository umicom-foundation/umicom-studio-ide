/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/build/diagnostic_parsers.c
 *
 * PURPOSE:
 *   Implement resilient, GLib-only parsers that normalize diagnostic output
 *   from common toolchains (GCC/Clang/MSVC). The result can be routed to any
 *   UI or logging surface via the generic output sink interfaces.
 *
 * WHY THIS FILE EXISTS:
 *   Historically, parsing lived in UI panes and included their headers. That
 *   tight coupling complicated reuse from the build runner. We now depend only
 *   on src/include/umi_output_sink.h and GLib—no panes/* includes.
 *
 * GUARANTEES:
 *   - No GTK usage.
 *   - Tolerates minor format variations (missing columns, extra whitespace).
 *   - Never crashes on malformed input; returns FALSE if not recognized.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                 /* GRegex, g_strdup, g_strstrip, etc.              */
#include "diagnostic_parsers.h"   /* public API + UmiDiagParseResult                 */

/*-----------------------------------------------------------------------------
 * Internal helpers
 *---------------------------------------------------------------------------*/

/* Small utility to map a token to UmiDiagSeverity.
 * Accepts "fatal error", "error", "warning", "note" (case-insensitive).
 * Defaults to UMI_DIAG_ERROR if token is NULL or unrecognized because most
 * toolchains only emit parse-target lines for noteworthy severities.
 */
static UmiDiagSeverity
sev_from_token(const char *tok)
{
  if (!tok) return UMI_DIAG_ERROR;

  if (g_ascii_strncasecmp(tok, "fatal error", 11) == 0) return UMI_DIAG_ERROR;
  if (g_ascii_strncasecmp(tok, "error", 5) == 0)        return UMI_DIAG_ERROR;
  if (g_ascii_strncasecmp(tok, "warning", 7) == 0)      return UMI_DIAG_WARNING;
  if (g_ascii_strncasecmp(tok, "note", 4) == 0)         return UMI_DIAG_NOTE;

  return UMI_DIAG_ERROR; /* conservative default */
}

/* Zero 'out' safely so callers never see garbage if we early-return. */
static inline void
reset_result(UmiDiagParseResult *out)
{
  if (!out) return;
  out->file = NULL;
  out->line = 0;
  out->col  = 0;
  out->sev  = UMI_DIAG_ERROR;
  out->msg  = NULL;
}

/* Assign helpers with g_strdup to keep all ownership uniform. */
static inline void
assign_result(UmiDiagParseResult *out, const char *file, int line, int col,
              UmiDiagSeverity sev, const char *msg)
{
  if (!out) return;
  out->file = file ? g_strdup(file) : NULL;
  out->line = line > 0 ? line : 0;
  out->col  = col  > 0 ? col  : 0;
  out->sev  = sev;
  out->msg  = msg  ? g_strdup(msg)  : NULL;
}

/*-----------------------------------------------------------------------------
 * GCC/Clang
 *   Typical lines:
 *     path/to/file.c:123:45: error: something bad happened
 *     path/to/file.c:123: warning: minor issue
 *---------------------------------------------------------------------------*/
gboolean
umi_diag_parse_gcc_clang(const char *s, UmiDiagParseResult *out)
{
  /* Validate input early. */
  if (!s || !out) return FALSE;
  reset_result(out);

  /* Regex notes:
   *   (1) file path   : group 1
   *   (2) line number : group 2
   *   (3) column      : optional group 3
   *   (4) severity    : group 4
   *   (5) message     : group 5
   *
   * Handles both "file:line:col:" and "file:line:" variants.
   */
  static GRegex *rx = NULL;
  if (G_UNLIKELY(rx == NULL)) {
    rx = g_regex_new(
      "^(.+?):"                  /* file (greedy, up to colon)                 */
      "([0-9]+)"                 /* line                                       */
      "(?::([0-9]+))?"           /* optional :col                              */
      "\\s*:\\s*"                /* colon with optional surrounding spaces     */
      "([A-Za-z ]+?)"            /* severity token (e.g., 'error', 'fatal error') */
      "\\s*:\\s*"                /* : after severity                           */
      "(.*)$",                   /* message                                    */
      G_REGEX_OPTIMIZE | G_REGEX_RAW, 0, NULL);
  }

  GMatchInfo *mi = NULL;
  gboolean matched = g_regex_match(rx, s, 0, &mi);
  if (!matched) {
    if (mi) g_match_info_free(mi);
    return FALSE;
  }

  gchar *file = g_match_info_fetch(mi, 1);
  gchar *line = g_match_info_fetch(mi, 2);
  gchar *col  = g_match_info_fetch(mi, 3);
  gchar *sev  = g_match_info_fetch(mi, 4);
  gchar *msg  = g_match_info_fetch(mi, 5);

  int ln = line ? (int)g_ascii_strtoll(line, NULL, 10) : 0;
  int cl = col  ? (int)g_ascii_strtoll(col,  NULL, 10) : 0;
  g_strstrip(sev);
  g_strstrip(msg);

  assign_result(out, file, ln, cl, sev_from_token(sev), msg);

  g_free(file); g_free(line); g_free(col); g_free(sev); g_free(msg);
  g_match_info_free(mi);
  return TRUE;
}

/*-----------------------------------------------------------------------------
 * MSVC
 *   Typical lines:
 *     C:\path\file.cpp(10,20): error C1234: message details
 *     C:\path\file.cpp(10): warning C5678: message
 *---------------------------------------------------------------------------*/
gboolean
umi_diag_parse_msvc(const char *s, UmiDiagParseResult *out)
{
  if (!s || !out) return FALSE;
  reset_result(out);

  /* Regex notes:
   *   (1) file path   : group 1
   *   (2) line        : group 2
   *   (3) col         : optional group 3
   *   (4) severity    : group 4
   *   (5) message     : group 5
   *
   * The 'code' token (like C1234) is intentionally ignored here; keep the
   * normalized message minimal. Callers can extract codes later if needed.
   */
  static GRegex *rx = NULL;
  if (G_UNLIKELY(rx == NULL)) {
    rx = g_regex_new(
      "^(.+?)\\("                /* file */
      "([0-9]+)"                 /* line */
      "(?:,([0-9]+))?"           /* optional ,col */
      "\\)\\s*:\\s*"             /* ) : */
      "([A-Za-z ]+?)"            /* severity token ('error', 'warning', 'fatal error') */
      "\\s+[A-Za-z]?[0-9]*\\s*:\\s*" /* optional code like C1234: */
      "(.*)$",                   /* message */
      G_REGEX_OPTIMIZE | G_REGEX_RAW, 0, NULL);
  }

  GMatchInfo *mi = NULL;
  gboolean matched = g_regex_match(rx, s, 0, &mi);
  if (!matched) {
    if (mi) g_match_info_free(mi);
    return FALSE;
  }

  gchar *file = g_match_info_fetch(mi, 1);
  gchar *line = g_match_info_fetch(mi, 2);
  gchar *col  = g_match_info_fetch(mi, 3);
  gchar *sev  = g_match_info_fetch(mi, 4);
  gchar *msg  = g_match_info_fetch(mi, 5);

  int ln = line ? (int)g_ascii_strtoll(line, NULL, 10) : 0;
  int cl = col  ? (int)g_ascii_strtoll(col,  NULL, 10) : 0;
  g_strstrip(sev);
  g_strstrip(msg);

  assign_result(out, file, ln, cl, sev_from_token(sev), msg);

  g_free(file); g_free(line); g_free(col); g_free(sev); g_free(msg);
  g_match_info_free(mi);
  return TRUE;
}

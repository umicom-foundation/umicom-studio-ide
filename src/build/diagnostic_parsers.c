/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/diagnostic_parsers.c
 *
 * PURPOSE:
 *   Heuristically parse one line of tool output into a structured diagnostic.
 *   This keeps the build subsystem independent from any UI widgets.  The
 *   output struct (UmiDiagParseResult) is plain C and lives in the header.
 *
 * DESIGN PRINCIPLES:
 *   - Pure functions: no globals, no static mutable state.
 *   - Zero Gtk dependencies; only GLib string helpers.
 *   - Be conservative: if we can't parse with confidence, return FALSE.
 *
 * INPUTS SUPPORTED (examples):
 *   GCC/Clang:
 *     "src/foo.c:10:7: warning: variable ‘x’ set but not used [-Wunused-but-set-variable]"
 *     "src/main.c:42: error: ‘BAR’ undeclared (first use in this function)"
 *   MSVC:
 *     "C:\\proj\\src\\foo.c(10,7): error C1234: Something bad happened"
 *   Generic note:
 *     "note: previous declaration is here"
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include "build/include/diagnostic_parsers.h"
#include <string.h> /* strlen, strchr, strrchr                                        */

/*---------------------------------------------------------------------------
 * Internal helpers — tiny utilities kept in this file only.
 *---------------------------------------------------------------------------*/

/* Convert an ASCII token to UmiDiagSeverity (case-insensitive). */
static UmiDiagSeverity sev_from_token(const char *t){
    if (!t) return UMI_DIAG_ERROR; /* conservative default when token missing */
    if (g_ascii_strncasecmp(t, "warning", 7) == 0) return UMI_DIAG_WARNING;
    if (g_ascii_strncasecmp(t, "note",    4) == 0) return UMI_DIAG_NOTE;
    return UMI_DIAG_ERROR;
}

/* Parse a positive integer; returns 0 if parsing fails. */
static int parse_int_or_zero(const char *s){
    if (!s) return 0;
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (end == s || v <= 0 || v > INT_MAX) return 0;
    return (int)v;
}

/*---------------------------------------------------------------------------
 * GCC/Clang-like:  file:line:col: <severity>:
 *   We accept variants:
 *     file:line:col: warning: msg
 *     file:line:    error:   msg
 *---------------------------------------------------------------------------*/
gboolean umi_dparse_try_gcc_like(const char *line, UmiDiagParseResult *out){
    if (!line || !out) return FALSE;

    /* Fast rejection: must contain at least two ':' separators. */
    const char *first_colon = strchr(line, ':');
    if (!first_colon) return FALSE;
    const char *second_colon = strchr(first_colon + 1, ':');
    if (!second_colon) return FALSE;

    /* Back up to file start; we accept drive letter on Windows "C:\..." */
    const char *p = line;
    /* Extract file substring [p, first_colon) */
    gchar *file = g_strndup(p, (gsize)(first_colon - p));
    if (!file || *file == '\0') { g_free(file); return FALSE; }

    /* line number is between first_colon+1 and second_colon */
    int lineno = parse_int_or_zero(first_colon + 1);
    if (lineno <= 0) { g_free(file); return FALSE; }

    /* Optional column: there may be third colon. */
    const char *third_colon = strchr(second_colon + 1, ':');
    int col = 0;
    const char *after_pos = second_colon + 1;
    if (third_colon){
        col = parse_int_or_zero(second_colon + 1);
        after_pos = third_colon + 1;
    }

    /* After the position, we expect "<sev>:" then a message. */
    while (*after_pos == ' ') after_pos++;
    /* Grab the severity token up to next ':' or space. */
    const char *sev_end = strchr(after_pos, ':');
    if (!sev_end) { g_free(file); return FALSE; }
    gchar *sev_tok = g_strndup(after_pos, (gsize)(sev_end - after_pos));
    for (char *q = sev_tok; *q; ++q) if (*q == ' ') { *q = '\0'; break; }

    UmiDiagSeverity sev = sev_from_token(sev_tok);
    g_free(sev_tok);

    /* Skip ':' and whitespace to get the message text. */
    const char *msg_start = sev_end + 1;
    while (*msg_start == ' ') msg_start++;
    if (!*msg_start){ g_free(file); return FALSE; }

    out->sev     = sev;
    out->file    = file;
    out->line    = lineno;
    out->column  = col;
    out->message = g_strdup(msg_start);
    return TRUE;
}

/*---------------------------------------------------------------------------
 * MSVC-like:  file(line,col): severity <code>: message
 *
 * We accept:
 *   "path\\to\\file.c(12,7): error C1234: something"
 *   "path\\to\\file.c(12): warning C4996: something"
 *---------------------------------------------------------------------------*/
gboolean umi_dparse_try_msvc_like(const char *line, UmiDiagParseResult *out){
    if (!line || !out) return FALSE;

    const char *lparen = strrchr(line, '(');
    const char *rparen = lparen ? strchr(lparen, ')') : NULL;
    const char *after  = rparen  ? strchr(rparen,  ':') : NULL;
    if (!lparen || !rparen || !after) return FALSE;

    /* File is up to the '(' */
    gchar *file = g_strndup(line, (gsize)(lparen - line));
    if (!file || *file == '\0') { g_free(file); return FALSE; }

    /* Extract line and optional column "line[,col]" */
    gchar *pos   = g_strndup(lparen + 1, (gsize)(rparen - lparen - 1));
    gchar **bits = g_strsplit(pos, ",", 2);
    int lineno = parse_int_or_zero(bits[0]);
    int col    = bits[1] ? parse_int_or_zero(bits[1]) : 0;
    g_strfreev(bits);
    g_free(pos);
    if (lineno <= 0) { g_free(file); return FALSE; }

    /* After the ')', format is ": <sev> <code>: message" */
    const char *sev_start = after + 1;
    while (*sev_start == ' ') sev_start++;
    const char *sev_end = strchr(sev_start, ' ');
    if (!sev_end){ g_free(file); return FALSE; }
    gchar *sev_tok = g_strndup(sev_start, (gsize)(sev_end - sev_start));

    UmiDiagSeverity sev = sev_from_token(sev_tok);
    g_free(sev_tok);

    /* Skip to colon after the (optional) error code and take the remainder */
    const char *msg_colon = strchr(sev_end, ':');
    if (!msg_colon){ g_free(file); return FALSE; }
    const char *msg = msg_colon + 1;
    while (*msg == ' ') msg++;

    out->sev     = sev;
    out->file    = file;
    out->line    = lineno;
    out->column  = col;
    out->message = g_strdup(msg);
    return TRUE;
}

/*---------------------------------------------------------------------------
 * Generic "note:" detector — useful for follow-up lines lacking file:line.
 *---------------------------------------------------------------------------*/
gboolean umi_dparse_try_generic_note(const char *line, UmiDiagParseResult *out){
    if (!line || !out) return FALSE;
    const char *p = line;
    while (*p == ' ') p++;
    if (g_ascii_strncasecmp(p, "note:", 5) != 0) return FALSE;

    out->sev     = UMI_DIAG_NOTE;
    out->file    = NULL;
    out->line    = 0;
    out->column  = 0;
    out->message = g_strdup(p + 5);
    /* Trim leading spaces from message */
    while (out->message && *out->message == ' ') {
        memmove(out->message, out->message + 1, strlen(out->message));
    }
    return TRUE;
}

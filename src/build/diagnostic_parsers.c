/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/build/diagnostic_parsers.c
 *
 * PURPOSE:
 *   Implements build output normalization into UmiDiag.
 *
 * DESIGN:
 *   Keeps parsing logic isolated; no UI/GTK includes here.
 *
 * API:
 *   (See function docs below.)
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "diagnostic_parsers.h"

/*  INTERNALS
 *  ────────────────────────────────────────────────────────────────────────────
 *  The parser can switch strategies based on tool_name. In a later stage we
 *  can plug a small table of regexes or fast scanners per tool.
 */
struct _UmiDiagParser {
    gchar *tool_name;  /* lowercase just for comparison */
};

static gboolean parse_generic(const char *line, UmiDiag **out);

/*  NEW / FREE
 *  ────────────────────────────────────────────────────────────────────────────
 *  Allocate and initialize a parser for a given tool.
 */
UmiDiagParser *umi_diag_parser_new(const char *tool_name) {
    UmiDiagParser *p = g_new0(UmiDiagParser, 1);
    if (tool_name) p->tool_name = g_ascii_strdown(tool_name, -1);
    return p;
}

void umi_diag_parser_free(UmiDiagParser *p) {
    if (!p) return;
    g_free(p->tool_name);
    g_free(p);
}

/*  FEED LINE
 *  ────────────────────────────────────────────────────────────────────────────
 *  We dispatch to the appropriate routine based on tool. For Stage-1/2, keep
 *  a generic parser that recognizes common patterns like:
 *      file:line:col: error/warning: message
 *      path\file(line,col): error C1234: message
 */
gboolean umi_diag_parser_feed_line(UmiDiagParser *p, const char *raw_line, UmiDiag **out) {
    if (!p || !raw_line || !out) return FALSE;
    *out = NULL;

    /* NOTE: Expandable dispatch. For now, use generic parser. */
    if (parse_generic(raw_line, out)) return TRUE;

    return FALSE;
}

/*  GENERIC PARSER
 *  ────────────────────────────────────────────────────────────────────────────
 *  This parser is intentionally simple and conservative. It tries to extract:
 *    - path
 *    - line/column (if present)
 *    - severity (error/warning/note)
 *    - message
 *  It avoids false positives by requiring a known severity keyword.
 */
static gboolean parse_generic(const char *line, UmiDiag **out) {
    /* Quick scan for severity keyword first */
    const char *sev = NULL;
    UmiDiagSeverity s = UMI_DIAG_NOTE; /* default lowest */
    if ((sev = g_strstr_len(line, -1, "error")))   s = UMI_DIAG_ERROR;
    else if ((sev = g_strstr_len(line, -1, "warning"))) s = UMI_DIAG_WARNING;
    else if ((sev = g_strstr_len(line, -1, "note")))    s = UMI_DIAG_NOTE;
    else return FALSE;

    /* Extract file and line if "file:line:" pattern exists */
    gchar *file = NULL;
    gint   lineno = 0;
    {
        /* naive split up to first ':' */
        const char *c = strchr(line, ':');
        if (c) {
            file = g_strndup(line, c - line);
            /* After file:, try parse integer line number */
            const char *p = c + 1;
            char *endp = NULL;
            long v = strtol(p, &endp, 10);
            if (endp && (*endp == ':' || *endp == ',' || *endp == ' ')) {
                lineno = (int)v;
            }
        }
    }

    /* Extract message after severity keyword */
    const char *msg = sev;
    /* Advance past severity word and possible colon/space */
    while (*msg && *msg != ':' && *msg != ' ') msg++;
    while (*msg == ':' || *msg == ' ') msg++;

    UmiDiag *d = g_new0(UmiDiag, 1);
    d->severity = s;
    d->line     = lineno;
    d->column   = 0;
    d->file     = file ? file : g_strdup("");
    d->message  = g_strdup(msg ? msg : line);

    *out = d;
    return TRUE;
}

/*  END OF FILE */

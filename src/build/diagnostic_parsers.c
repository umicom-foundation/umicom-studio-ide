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
/*------------------------------- Lifecycle ----------------------------------*/
UmiDiagParser *umi_diag_parser_new(const char *tool_name)
{
/* Allocate zeroed parser object (GLib allocator; sets fields to 0/NULL). */
UmiDiagParser *p = g_new0(UmiDiagParser, 1);


/* If caller supplied a tool name, normalise to lowercase for comparisons. */
if (tool_name) p->tool_name = g_ascii_strdown(tool_name, -1);


return p; /* opaque handle to caller */
}


void umi_diag_parser_free(UmiDiagParser *p)
{
if (!p) return; /* safe on NULL to simplify calling code */
g_free(p->tool_name); /* free any tool name copy */
g_free(p); /* free parser */
}


/*------------------------------- Main Feed ----------------------------------*/
gboolean umi_diag_parser_feed_line(UmiDiagParser *p, const char *raw_line, UmiDiag **out)
{
/* Pseudo-code / intent:
* 1) Validate inputs; we must have an output slot (*out) to fill.
* 2) Dispatch by tool if needed (e.g., if p->tool_name == "clang").
* 3) Use conservative generic parser for now; extend later.
*/
(void)p; /* not used in generic mode yet */
if (!raw_line || !out) return FALSE; /* minimal contract */
*out = NULL; /* ensure clean on failure */


if (parse_generic(raw_line, out)) /* try generic */
return TRUE; /* parsed a diagnostic */


return FALSE; /* no match */
}


/*------------------------------ Generic Parser ------------------------------*/
static gboolean parse_generic(const char *line, UmiDiag **out)
{
/* Conservative recognition of common compiler formats:
* file:line:col: error|warning|note: message
* file:line: error|warning|note: message
* error: message (no file) — mirrored to Problems with empty file
*/
if (!line || !out) return FALSE;


/* Detect severity keyword; choose lowest cost scans first. */
UmiDiagSeverity sev = UMI_DIAG_NOTE; /* default to note */
const char *sevpos = NULL; /* where the keyword occurs */


if ((sevpos = g_strrstr(line, " error:")) || g_str_has_prefix(line, "error:"))
sev = UMI_DIAG_ERROR;
else if ((sevpos = g_strrstr(line, " warning:")) || g_str_has_prefix(line, "warning:"))
sev = UMI_DIAG_WARNING;
else if ((sevpos = g_strrstr(line, " note:")) || g_str_has_prefix(line, "note:"))
sev = UMI_DIAG_NOTE;
else
return FALSE; /* No known severity => not a diagnostic line. */


/* Try to extract a leading file:line:... prefix. */
gchar *file = NULL; /* default: empty -> means unknown */
gint lineno = 0;
{
const char *c = strchr(line, ':'); /* first colon */
if (c) {
file = g_strndup(line, c - line); /* copy file-like prefix */
const char *p = c + 1; /* after first colon */
char *endp = NULL; /* strtol end */
long v = strtol(p, &endp, 10); /* parse line number */
if (endp && (*endp == ':' || *endp == ',' || *endp == ' '))
lineno = (int)v; /* accept integer line */
}
}


/* Message: from keyword position onward, trim leading spaces. */
const char *msg = sevpos ? (sevpos + 1) : line; /* +1 to skip leading space */
while (*msg == ' ' || *msg == '\t') msg++;


/* Allocate result; all strings duplicated for ownership clarity. */
UmiDiag *d = g_new0(UmiDiag, 1);
d->severity = sev;
d->line = lineno;
d->column = 0;
d->file = file ? file : g_strdup("");
d->message = g_strdup(msg);


*out = d; /* caller now owns and must free via umi_diag_free() */
return TRUE;
}

/*  END OF FILE */

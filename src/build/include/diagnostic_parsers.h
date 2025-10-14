/*-----------------------------------------------------------------------------
* Umicom Studio IDE : OpenSource IDE for developers and Content Creators
* Repository: https://github.com/umicom-foundation/umicom-studio-ide
* File: src/build/include/diagnostic_parsers.h
*
* PURPOSE:
* Thin, UI-agnostic interface for converting raw tool output lines into
* structured UmiDiag records. Keeps the parser opaque to callers.
*
* DESIGN:
* - Opaque UmiDiagParser object, created/fed/freed by three functions.
* - Each successful feed returns a heap-allocated UmiDiag the caller owns.
* - Parser knows about severities but not UI widgets.
*
* API (typical):
* UmiDiagParser *umi_diag_parser_new(const char *tool_name);
* gboolean umi_diag_parser_feed_line(UmiDiagParser*, const char *raw, UmiDiag **out);
* void umi_diag_parser_free(UmiDiagParser*);
*
* Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
*---------------------------------------------------------------------------*/
#pragma once


#include <glib.h>
#include "umi_diag_types.h" /* UmiDiag */
#include "umi_diagnostics.h" /* UmiDiagSeverity */


/* Opaque parser type — callers don’t see internals (loose coupling). */
typedef struct _UmiDiagParser UmiDiagParser;


/* Create a parser; pass tool_name (e.g., "ninja", "clang", NULL for generic). */
UmiDiagParser *umi_diag_parser_new(const char *tool_name);


/* Feed one raw output line; returns TRUE if a diagnostic was parsed.
* When TRUE, *out is set to a newly-allocated UmiDiag the caller must free.
*/
gboolean umi_diag_parser_feed_line(UmiDiagParser *p, const char *raw_line, UmiDiag **out);


/* Destroy a parser and free internal allocations (safe on NULL). */
void umi_diag_parser_free(UmiDiagParser *p);
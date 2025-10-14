/*---------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/diagnostic_parsers.h
 *
 * PURPOSE:
 *   Thin, UI-agnostic interface for converting raw tool output lines into
 *   structured UmiDiag records. Keeps the parser opaque to callers.
 *
 * DESIGN:
 *   - Opaque UmiDiagParser object, created/fed/freed by three functions.
 *   - Each successful feed returns a heap-allocated UmiDiag the caller owns.
 *   - Parser knows about severities but not UI widgets.
 *
 * API (typical):
 *   UmiDiagParser *umi_diag_parser_new(void);
 *   gboolean umi_diag_parser_feed_line(UmiDiagParser*, const char *raw, UmiDiag **out);
 *   void umi_diag_parser_free(UmiDiagParser*);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef DIAGNOSTIC_PARSERS_H
#define DIAGNOSTIC_PARSERS_H

#include <glib.h>
#include "umi_diag_types.h"   /* UmiDiag */
#include "umi_diagnostics.h"  /* UmiDiagSeverity */

/* Forward-declared opaque parser type */
typedef struct UmiDiagParser UmiDiagParser;

/* Create/free parser */
UmiDiagParser *umi_diag_parser_new(void);
void           umi_diag_parser_free(UmiDiagParser *p);

/* Feed a raw line; returns TRUE if a complete diagnostic was parsed.
 * When TRUE, *out is set to a newly-allocated UmiDiag (caller frees with umi_diag_free()).
 *//*-
gboolean umi_diag_parser_feed_line(UmiDiagParser *p, const char *raw_line, UmiDiag **out);

#endif /* DIAGNOSTIC_PARSERS_H */
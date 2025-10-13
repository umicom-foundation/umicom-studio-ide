/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/build/include/diagnostic_parsers.h
 *
 * PURPOSE:
 *   Public declarations for normalizing build tool output into canonical UmiDiag records.
 *
 * DESIGN:
 *   UI-free. Pure parsing utilities used by BuildRunner and DiagnosticsRouter.
 *
 * API:
 *   UmiDiagParser, umi_diag_parser_new, umi_diag_parser_free, umi_diag_parser_feed_line
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#pragma once

/*  RATIONALE
 *  ────────────────────────────────────────────────────────────────────────────
 *  Downstream modules (core router, problems pane) do not want to know about
 *  the raw build tool line formats. This module converts raw text lines into
 *  UmiDiag records that capture severity, file, range, and message.
 *
 *  We include ONLY the canonical diagnostics types:
 */
#include "umi_output_sink.h"  /* brings UmiDiag/UmiDiagSeverity */

/*  FORWARD TYPES
 *  ────────────────────────────────────────────────────────────────────────────
 *  Opaque parser instance. Internals are private to diagnostic_parsers.c
 */
typedef struct _UmiDiagParser UmiDiagParser;

/*  LIFETIME
 *  ────────────────────────────────────────────────────────────────────────────
 *  'tool_name' is a hint (e.g., "gcc", "clang", "msvc", "ninja") to enable
 *  specialized parse paths. Unknown tools fall back to heuristics.
 */
UmiDiagParser *umi_diag_parser_new (const char *tool_name);
/*  Destroy a parser and free resources. */
void           umi_diag_parser_free(UmiDiagParser *p);

/*  FEED LINES
 *  ────────────────────────────────────────────────────────────────────────────
 *  Feed a single raw output line from the build tool. If a diagnostic is
 *  recognized, '*out' is set to a newly allocated UmiDiag which the caller
 *  must free with 'umi_diag_free' (defined alongside UmiDiag).
 *
 *  Returns TRUE if a diagnostic was produced for this line; FALSE otherwise.
 *  Non-diagnostic lines should simply return FALSE.
 */
gboolean       umi_diag_parser_feed_line(UmiDiagParser *p,
                                         const char   *raw_line,
                                         UmiDiag     **out);

/*  THREADING
 *  ────────────────────────────────────────────────────────────────────────────
 *  Parsers are not thread-safe; feed them on the same thread that created them.
 */

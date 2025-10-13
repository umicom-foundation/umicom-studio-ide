/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/diagnostic_parsers.h
 *
 * PURPOSE:
 *   Helpers to parse compiler/tool output lines into structured diagnostics
 *   that the UI can then display in a Problems pane (or any other consumer).
 *
 * DECOUPLING:
 *   - Depends only on umi_output_sink.h for the UmiDiagSeverity enum.
 *
 * API:
 *   typedef struct UmiParsedDiag { ... } UmiParsedDiag;
 *   gboolean umi_diag_parse_gcc(const char *line, UmiParsedDiag *out);
 *   gboolean umi_diag_parse_msvc(const char *line, UmiParsedDiag *out);
 *   gboolean umi_diag_parse_clang(const char *line, UmiParsedDiag *out);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_DIAGNOSTIC_PARSERS_H
#define UMI_DIAGNOSTIC_PARSERS_H

#include <glib.h>
#include "src/include/umi_output_sink.h"  /* UmiDiagSeverity */

G_BEGIN_DECLS

typedef struct UmiParsedDiag
{
    UmiDiagSeverity sev;     /* note/warning/error */
    char           *file;    /* optional */
    int             line;    /* 1-based; 0 -> unknown */
    char           *message; /* never NULL (empty OK) */
} UmiParsedDiag;

gboolean umi_diag_parse_gcc  (const char *line, UmiParsedDiag *out);
gboolean umi_diag_parse_msvc (const char *line, UmiParsedDiag *out);
gboolean umi_diag_parse_clang(const char *line, UmiParsedDiag *out);

void     umi_parsed_diag_clear(UmiParsedDiag *d);

G_END_DECLS

#endif /* UMI_DIAGNOSTIC_PARSERS_H */

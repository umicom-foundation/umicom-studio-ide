/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/diagnostic_parsers.h
 *
 * PURPOSE:
 *   Parse raw compiler/linker output lines into structured diagnostics that
 *   the rest of the app can consume (Problems pane, output sinks, etc.).
 *
 * DECOUPLING:
 *   - This header is intentionally UI-agnostic and sink-agnostic.  It only
 *     exposes a plain C API and types from src/include/umi_diagnostics.h.
 *   - Callers may forward results to any UmiOutputSink or custom handler.
 *
 * API OVERVIEW:
 *   - umi_dparse_try_gcc_like():     GCC/Clang-style "file:line:col: warning: msg"
 *   - umi_dparse_try_msvc_like():    MSVC "file(line,col): error C####: msg"
 *   - umi_dparse_try_generic_note(): Generic "note:" detector to enrich context
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_DIAGNOSTIC_PARSERS_H
#define UMI_DIAGNOSTIC_PARSERS_H

#include <glib.h>
#include "umi_diagnostics.h"  /* canonical UmiDiagSeverity */

/*---------------------------------------------------------------------------
 * Result value object for one parsed diagnostic line.
 *---------------------------------------------------------------------------*/
typedef struct UmiDiagParseResult_ {
    UmiDiagSeverity sev;     /* severity of the message                       */
    char           *file;    /* path to source file (g_strdup'd)              */
    int             line;    /* 1-based source line                           */
    int             column;  /* 1-based source column (0 if unavailable)      */
    char           *message; /* human-readable message (g_strdup'd)           */
} UmiDiagParseResult;

/*---------------------------------------------------------------------------
 * Memory management helpers.
 *---------------------------------------------------------------------------*/
static inline void umi_dparse_result_clear(UmiDiagParseResult *r){
    if (!r) return;
    g_clear_pointer(&r->file, g_free);
    g_clear_pointer(&r->message, g_free);
    r->line = 0; r->column = 0; r->sev = UMI_DIAG_NOTE;
}

/*---------------------------------------------------------------------------
 * Parse attempts — return TRUE if 'line' matched and 'out' was filled.
 *---------------------------------------------------------------------------*/
gboolean umi_dparse_try_gcc_like    (const char *line, UmiDiagParseResult *out);
gboolean umi_dparse_try_msvc_like   (const char *line, UmiDiagParseResult *out);
gboolean umi_dparse_try_generic_note(const char *line, UmiDiagParseResult *out);

#endif /* UMI_DIAGNOSTIC_PARSERS_H */

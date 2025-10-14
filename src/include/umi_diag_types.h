/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_diag_types.h
 *
 * PURPOSE:
 *   Canonical diagnostic record shared across build runners, parsers,
 *   and UI problem lists. Keeps a single, minimal struct that other
 *   modules can copy or reference without circular includes.
 *
 * DESIGN:
 *   - Severity comes from umi_diagnostics.h (source of truth).
 *   - Strings are owned UTF-8; free with umi_diag_free().
 *   - Line/column are 1-based; 0 means "unknown".
 *
 * API (typical):
 *   typedef struct UmiDiag { ... } UmiDiag;
 *   void umi_diag_free(UmiDiag *d);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_DIAG_TYPES_H
#define UMI_DIAG_TYPES_H

#include <glib.h>
#include "umi_diagnostics.h" /* for UmiDiagSeverity */

typedef struct UmiDiag {
    UmiDiagSeverity severity;
    char           *file;    /* UTF-8 path (owned)  */
    char           *message; /* UTF-8 message       */
    unsigned        line;    /* 1-based, 0 = unknown */
    unsigned        column;  /* 1-based, 0 = unknown */
} UmiDiag;

/* Header-only safe free; OK even if parser also provides a non-static version */
static inline void umi_diag_free(UmiDiag *d) {
    if (!d) return;
    g_free(d->file);
    g_free(d->message);
    g_free(d);
}

#endif /* UMI_DIAG_TYPES_H */
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_diagnostics.h
 *
 * PURPOSE:
 *   Provide a single, canonical definition of diagnostics severity used
 *   throughout Umicom Studio IDE (build system, problems pane, console sink).
 *
 * DESIGN:
 *   - This header only defines the shared enum + tiny helpers.
 *   - Other modules MUST include this header instead of re-declaring enums.
 *   - Keeps modules loosely coupled: no UI types here.
 *
 * API:
 *   typedef enum UmiDiagSeverity { ... } UmiDiagSeverity;
 *   const char* umi_diag_severity_name(UmiDiagSeverity s);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_DIAGNOSTICS_H
#define UMI_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Centralized, single source-of-truth severity levels.
 * NOTE: Values stable — external logs and UI rely on these ordinals. */
typedef enum UmiDiagSeverity {
    UMI_DIAG_ERROR   = 0,  /* Highest priority: build-breaking / critical        */
    UMI_DIAG_WARNING = 1,  /* Medium priority: suspicious but not fatal          */
    UMI_DIAG_NOTE    = 2   /* Lowest priority: info, notes, hints                */
} UmiDiagSeverity;

/* Small helper for pretty-printing/logging. */
static inline const char* umi_diag_severity_name(UmiDiagSeverity s) {
    switch (s) {
        case UMI_DIAG_ERROR:   return "error";
        case UMI_DIAG_WARNING: return "warning";
        case UMI_DIAG_NOTE:    return "note";
        default:               return "unknown";
    }
}

#ifdef __cplusplus
}
#endif
#endif /* UMI_DIAGNOSTICS_H */

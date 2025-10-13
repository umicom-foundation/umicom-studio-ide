/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_diagnostics.h
 *
 * PURPOSE:
 *   Provide a single, canonical definition of diagnostics severity used
 *   throughout Umicom Studio IDE (build system, problems pane, console sink).
 *
 * WHY THIS EXISTS:
 *   Historically, multiple modules (e.g. problems list and output sink) each
 *   declared their own UmiDiagSeverity, sometimes with different *ordering*.
 *   That caused ODR-like conflicts and subtle sorting discrepancies.
 *   This header centralizes the enum to prevent redefinitions and guarantees
 *   consistent mapping between tooling (ninja/msvc/gcc) and our UI layers.
 *
 * USAGE:
 *   - Include this header anywhere you need the severity type or names.
 *   - Do NOT redeclare UmiDiagSeverity locally anywhere else.
 *   - If you previously declared the enum in a module header, remove it and
 *     include this file instead.
 *
 * THREADING:
 *   - None — it is a pure type definition.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_DIAGNOSTICS_H
#define UMI_DIAGNOSTICS_H

/*---------------------------------------------------------------------------
 * Minimal dependency: GLib for gboolean where helpful in signatures that
 * might consume this header.  (Safe and ubiquitous in our codebase.)
 *---------------------------------------------------------------------------*/
#include <glib.h>

/*---------------------------------------------------------------------------
 * Canonical diagnostics severity (stable order)
 *
 * NOTE:
 *   The ordinal values are part of the "contract" used by various sort and
 *   filter routines across the app.  Keep NOTE < WARNING < ERROR.
 *---------------------------------------------------------------------------*/
typedef enum UmiDiagSeverity_ {
    UMI_DIAG_NOTE    = 0,   /* Lowest severity: general note/info            */
    UMI_DIAG_WARNING = 1,   /* Medium severity: potential issues              */
    UMI_DIAG_ERROR   = 2    /* Highest severity: build failed or hard error   */
} UmiDiagSeverity;

/*---------------------------------------------------------------------------
 * Helpers for readability — tiny inline predicates are header-only so they
 * impose no linkage/ABI burden and keep callsites tidy.
 *---------------------------------------------------------------------------*/
static inline gboolean umi_diag_is_error   (UmiDiagSeverity s){ return s == UMI_DIAG_ERROR; }
static inline gboolean umi_diag_is_warning (UmiDiagSeverity s){ return s == UMI_DIAG_WARNING; }
static inline gboolean umi_diag_is_note    (UmiDiagSeverity s){ return s == UMI_DIAG_NOTE; }

#endif /* UMI_DIAGNOSTICS_H */

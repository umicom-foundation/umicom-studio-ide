/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/include/umi_diagnostics.h
 *
 * PURPOSE:
 *   Canonical, project-wide definitions for build/runtime diagnostics
 *   (severity enum + UmiDiag record). Every module that emits or consumes
 *   diagnostics must use these types to stay ABI- and API-consistent.
 *
 * DESIGN:
 *   - This is the **single source of truth** for UmiDiagSeverity.
 *   - Lightweight, UI-agnostic, no GTK dependency — only GLib basics.
 *   - Safe defaults: all pointers may be NULL; numbers are size_t.
 *
 * API:
 *   typedef enum UmiDiagSeverity { ... } UmiDiagSeverity;
 *   typedef struct UmiDiag { ... } UmiDiag;
 *
 *   // Convenience helpers (header-only, inline):
 *   static inline const char *umi_diag_severity_name(UmiDiagSeverity s);
 *   static inline void        umi_diag_clear(UmiDiag *d);
 *
 * INTEGRATION NOTES:
 *   - UI consumers (Problems pane, output consoles) include this header
 *     *directly*, and must NOT redeclare the enum or record.
 *   - Sinks/routers that forward diagnostics should depend only on this file.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_USIDE_UMI_DIAGNOSTICS_H
#define UMICOM_USIDE_UMI_DIAGNOSTICS_H

#include <glib.h>   /* size_t, gboolean */

/*----------------------------------------------------------------------------
 * UmiDiagSeverity:
 *   Ordered from highest to lowest priority. The integer values are stable
 *   and may be persisted in logs or IPC, so do not change their numeric
 *   assignments without a migration plan.
 *--------------------------------------------------------------------------*/
typedef enum UmiDiagSeverity {
    UMI_DIAG_ERROR   = 0,  /* Highest priority: build-breaking / critical        */
    UMI_DIAG_WARNING = 1,  /* Medium priority: suspicious but not fatal          */
    UMI_DIAG_NOTE    = 2   /* Lowest priority: info, notes, hints                */
} UmiDiagSeverity;

/*----------------------------------------------------------------------------
 * UmiDiag:
 *   A single diagnostic event. All fields are optional; consumers must guard
 *   NULL strings and treat (line,column)==0 as "unknown".
 *
 *   file    : path of the file related to the diag (UTF-8; may be NULL/"")
 *   line    : 1-based line number, or 0 if unknown
 *   column  : 1-based column number, or 0 if unknown
 *   severity: one of UMI_DIAG_{ERROR,WARNING,NOTE}
 *   message : human-readable explanation (UTF-8; may be NULL/"")
 *--------------------------------------------------------------------------*/
typedef struct UmiDiag {
    const char      *file;      /* not owned; producer retains ownership       */
    size_t           line;      /* 0 when unknown                             */
    size_t           column;    /* 0 when unknown                             */
    UmiDiagSeverity  severity;  /* enum above                                 */
    const char      *message;   /* not owned; may be NULL                     */
} UmiDiag;

/*----------------------------------------------------------------------------
 * Helpers (inline, header-only)
 *--------------------------------------------------------------------------*/

/* Convert severity to a short, user-friendly string. Never returns NULL. */
static inline const char *umi_diag_severity_name(UmiDiagSeverity s)
{
    switch (s) {
        case UMI_DIAG_ERROR:   return "error";
        case UMI_DIAG_WARNING: return "warning";
        case UMI_DIAG_NOTE:    return "note";
        default:               return "note";
    }
}

/* Reset a diag to safe "empty" defaults. */
static inline void umi_diag_clear(UmiDiag *d)
{
    if (!d) return;
    d->file = NULL;
    d->line = 0u;
    d->column = 0u;
    d->severity = UMI_DIAG_NOTE;
    d->message = NULL;
}

#endif /* UMICOM_USIDE_UMI_DIAGNOSTICS_H */

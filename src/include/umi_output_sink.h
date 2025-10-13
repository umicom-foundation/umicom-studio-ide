/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/include/umi_output_sink.h
 *
 * PURPOSE:
 *   Small, UI-agnostic interface used by build runners / tasks / tools to
 *   stream textual output lines and structured diagnostics to any consumer
 *   (console, problems pane, log file, etc.).
 *
 * DESIGN:
 *   - **Depends on** umi_diagnostics.h for UmiDiag + UmiDiagSeverity.
 *     (Do NOT re-declare the enum here — this was the source of the
 *      "redeclaration / conflicting types" compile error you hit.)
 *   - Plain C "strategy" object with two callbacks and a user pointer.
 *   - Header-only helpers make sink calls null-safe and concise.
 *
 * API:
 *   typedef struct UmiOutputSink { ... } UmiOutputSink;
 *
 *   // Safe helpers to emit data if callback present (no-ops otherwise):
 *   static inline void umi_sink_line (UmiOutputSink *s, const char *line);
 *   static inline void umi_sink_diag (UmiOutputSink *s, const UmiDiag *d);
 *   static inline void umi_sink_error(UmiOutputSink *s, const char *file,
 *                                     size_t line, size_t col, const char *msg);
 *
 * USAGE:
 *   - A consumer (e.g., Output pane) typically embeds a UmiOutputSink
 *     inside its instance and initializes the function pointers to its
 *     own methods. Producers get a pointer and simply call the helpers.
 *
 *   Example:
 *     static void my_line(void *u, const char *l) { ... }
 *     static void my_diag(void *u, const UmiDiag *d) { ... }
 *     UmiOutputSink sink = { .user=ctx, .on_line=my_line, .on_diag=my_diag };
 *     umi_sink_line(&sink, "hello");
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_USIDE_UMI_OUTPUT_SINK_H
#define UMICOM_USIDE_UMI_OUTPUT_SINK_H

#include <glib.h>
#include "umi_diagnostics.h"  /* <- single source of truth for UmiDiag types */

/*----------------------------------------------------------------------------
 * UmiOutputSink:
 *   Tiny vtable for streaming text lines and/or structured diagnostics.
 *   The object itself owns no memory; it just holds function pointers.
 *--------------------------------------------------------------------------*/
typedef struct UmiOutputSink {
    void (*on_line)(void *user, const char *line);      /* may be NULL */
    void (*on_diag)(void *user, const UmiDiag *diag);   /* may be NULL */
    void  *user;                                        /* opaque      */
} UmiOutputSink;

/* Emit a plain text line (safe on NULL sink or NULL callback). */
static inline void umi_sink_line(UmiOutputSink *s, const char *line)
{
    if (!s || !s->on_line) return;
    s->on_line(s->user, line ? line : "");
}

/* Emit a structured diagnostic (safe on NULLs). */
static inline void umi_sink_diag(UmiOutputSink *s, const UmiDiag *d)
{
    if (!s || !s->on_diag || !d) return;
    s->on_diag(s->user, d);
}

/* Convenience: emit an ERROR diagnostic quickly (alloc-free, caller strings). */
static inline void umi_sink_error(UmiOutputSink *s,
                                  const char *file, size_t line, size_t col,
                                  const char *msg)
{
    if (!s) return;
    UmiDiag d;
    d.file = file;
    d.line = line;
    d.column = col;
    d.severity = UMI_DIAG_ERROR;
    d.message = msg;
    umi_sink_diag(s, &d);
}

#endif /* UMICOM_USIDE_UMI_OUTPUT_SINK_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_output_sink.h
 *
 * PURPOSE:
 *   Define a small, GUI-agnostic "output sink" interface that the build
 *   subsystem can write to (stdout/stderr lines, begin/end groups, etc.).
 *   This allows build code to remain loosely coupled: it does not include
 *   headers from UI panes (console/problems) and vice versa.
 *
 * NOTES ON COUPLING:
 *   - Depends only on GLib and the canonical diagnostics enum in
 *     src/include/umi_diagnostics.h.
 *   - NO Gtk types or widget structs appear here.
 *
 * CHANGELOG (Design Rationale):
 *   - Centralized severity in umi_diagnostics.h to avoid enum duplication and
 *     redefinition errors previously seen when different modules declared their
 *     own severities with mismatched ordering.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_OUTPUT_SINK_H
#define UMI_OUTPUT_SINK_H

/*---------------------------------------------------------------------------
 * Minimal includes: keep this header lightweight to avoid deep dependency
 * chains.  We need GLib for gboolean/gpointer and the canonical severity.
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include "umi_diagnostics.h"

G_BEGIN_DECLS

/*---------------------------------------------------------------------------
 * UmiOutputSink:
 *   A tiny callback bundle used by CLI/build-facing code to emit structured
 *   lines and lifecycle signals.  Any UI or logger can implement this.
 *
 * IMPLEMENTATION GUIDELINES FOR SINKS:
 *   - All callbacks are optional; NULL is a no-op.
 *   - Implementations must be thread-aware if called off main thread.
 *---------------------------------------------------------------------------*/
typedef struct UmiOutputSink_ {
    /*-----------------------------------------------------------------------
     * Emit one logical line of output.
     *   - 'line' should be a single line without trailing newline.
     *   - 'is_stderr' distinguishes stdout vs stderr for styling.
     *-----------------------------------------------------------------------*/
    void (*write_line)(void *user, const char *line, gboolean is_stderr);

    /*-----------------------------------------------------------------------
     * Emit a parsed diagnostic (if available).
     *   - severity + file:line:col + message
     *   - File path may be absolute or workspace-relative depending on caller.
     *-----------------------------------------------------------------------*/
    void (*emit_diagnostic)(void *user,
                            UmiDiagSeverity sev,
                            const char     *file,
                            int             line,
                            int             column,
                            const char     *message);
    /*-----------------------------------------------------------------------
     * Lifecycle hooks (optional).
     *-----------------------------------------------------------------------*/
    void (*begin_group)(void *user, const char *label);
    void (*end_group)  (void *user);

    /*-----------------------------------------------------------------------
     * Opaque user pointer forwarded to every callback.
     *-----------------------------------------------------------------------*/
    void *user;
} UmiOutputSink;

/*---------------------------------------------------------------------------
 * Small helpers to safely invoke callbacks if the sink is partially filled.
 *---------------------------------------------------------------------------*/
static inline void umi_sink_write(UmiOutputSink *s, const char *line, gboolean is_stderr){
    if (s && s->write_line) s->write_line(s->user, line, is_stderr);
}
static inline void umi_sink_diag(UmiOutputSink *s, UmiDiagSeverity sev,
                                 const char *file, int line, int column, const char *msg){
    if (s && s->emit_diagnostic) s->emit_diagnostic(s->user, sev, file, line, column, msg);
}
static inline void umi_sink_begin(UmiOutputSink *s, const char *label){
    if (s && s->begin_group) s->begin_group(s->user, label);
}
static inline void umi_sink_end(UmiOutputSink *s){
    if (s && s->end_group) s->end_group(s->user);
}

G_END_DECLS

#endif /* UMI_OUTPUT_SINK_H */

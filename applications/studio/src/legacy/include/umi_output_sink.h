/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_output_sink.h
 *
 * PURPOSE:
 *   Abstract output sink that receives line-oriented text and diagnostics
 *   from subsystems (builder, runners, parsers) without depending on UI.
 *
 * DESIGN:
 *   - UI implements this (console pane, problems pane adapters, etc).
 *   - Core code only targets this interface (loosely coupled).
 *   - Uses UmiDiagSeverity from umi_diagnostics.h (single source-of-truth).
 *   - Uses GLib gboolean in callbacks to match existing call sites.
 *
 * API (typical):
 *   typedef struct UmiOutputSink UmiOutputSink;
 *   void umi_output_sink_append_line(UmiOutputSink *s, const char *utf8);
 *   void umi_output_sink_append_err_line(UmiOutputSink *s, const char *utf8);
 *   void umi_output_sink_append_diag(UmiOutputSink *s,
 *                                    UmiDiagSeverity sev,
 *                                    const char *file, int line, int col,
 *                                    const char *message);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_OUTPUT_SINK_H
#define UMI_OUTPUT_SINK_H

#include <glib.h>
#include <stdlib.h>
#include "umi_diag_types.h" /* UmiDiag, UmiDiagSeverity */

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*UmiOutputLineFn)(void *user, const char *line, gboolean is_err);
typedef void (*UmiOutputDiagFn)(void *user,
                                UmiDiagSeverity sev,
                                const char *file,
                                int line,
                                int column,
                                const char *message);

/* Concrete sink (not opaque) to match legacy field access in existing .c files */
typedef struct UmiOutputSink {
    void             *user;
    UmiOutputLineFn   on_line; /* may be NULL */
    UmiOutputDiagFn   on_diag; /* may be NULL */
} UmiOutputSink;

/* Header-only helpers (no .c needed) */
static inline UmiOutputSink *umi_output_sink_new(UmiOutputLineFn line_fn,
                                                 UmiOutputDiagFn diag_fn,
                                                 void *user) {
    UmiOutputSink *s = (UmiOutputSink*)malloc(sizeof *s);
    if (!s) return NULL;
    s->user = user;
    s->on_line = line_fn;
    s->on_diag = diag_fn;
    return s;
}

static inline void umi_output_sink_free(UmiOutputSink *s) {
    free(s);
}

static inline void umi_output_sink_append_line(UmiOutputSink *s, const char *utf8) {
    if (s && s->on_line) s->on_line(s->user, utf8 ? utf8 : "", FALSE);
}

/* For routing stderr lines explicitly as errors */
static inline void umi_output_sink_append_err_line(UmiOutputSink *s, const char *utf8) {
    if (s && s->on_line) s->on_line(s->user, utf8 ? utf8 : "", TRUE);
}

static inline void umi_output_sink_append_diag(UmiOutputSink *s,
                                               UmiDiagSeverity sev,
                                               const char *file,
                                               int line,
                                               int column,
                                               const char *msg) {
    if (s && s->on_diag) s->on_diag(s->user, sev, file ? file : "", line, column, msg ? msg : "");
}

/* Compatibility helper used by build_tasks.c: umi_output_sink_emit(s, &diag) */
static inline void umi_output_sink_emit(UmiOutputSink *s, const UmiDiag *d) {
    if (!s || !d) return;
    umi_output_sink_append_diag(s, d->severity,
                                d->file ? d->file : "",
                                (int)d->line,
                                (int)d->column,
                                d->message ? d->message : "");
}

#ifdef __cplusplus
}
#endif
#endif /* UMI_OUTPUT_SINK_H */
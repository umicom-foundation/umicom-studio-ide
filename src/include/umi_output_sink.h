/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_output_sink.h
 *
 * PURPOSE:
 *   A tiny, cross-module interface for "output sinks" — a loosely coupled
 *   callback used by producers (build, run, tools) to stream text lines to
 *   consumers (output console, problems pane, log window, etc.).
 *
 *   This header deliberately lives in:  src/include/
 *   which is already part of the global include path in CMake, so any
 *   submodule can include it without using fragile relative paths.
 *
 * MOTIVATION (ARCHITECTURE):
 *   Previously, files in src/build/ included pane-specific headers via:
 *       #include "../../panes/output/include/output_console.h"
 *       #include "../../panes/output/include/output_pipeline.h"
 *   That created bidirectional coupling and brittle relative paths.
 *   With this abstraction, the build system only depends on this header,
 *   and the UI layer provides an adapter that implements the callback.
 *
 * HOW IT WORKS:
 *   - Producers push complete, UTF-8 lines (without trailing '\n' required;
 *     adapters may normalize).
 *   - The 'is_err' flag lets a consumer style stderr differently.
 *   - The opaque 'user' pointer lets a consumer carry context (e.g. a
 *     UmiOutputConsole* instance, a GtkTextBuffer*, etc.).
 *
 *   The producer never includes any pane or GTK headers—only this file.
 *
 * THREADING:
 *   Callbacks are invoked on the GLib main context in this codebase.
 *   If a producer is multi-threaded, it must marshal into the main loop
 *   before calling a UI-backed sink.
 *
 * LIFETIME / OWNERSHIP:
 *   - The producer does not own 'user' and must not free it.
 *   - Strings passed to the sink are read-only for the duration of the call.
 *
 * VERSIONING:
 *   Keep this header minimal and stable. Add fields by adding new functions
 *   (e.g., a struct-based sink) rather than changing this signature.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_OUTPUT_SINK_H
#define UMI_OUTPUT_SINK_H

#include <glib.h>   /* gboolean, gpointer */

G_BEGIN_DECLS

/*-------------------------------------------------------------------------*/
/* UmiOutputSink
 *
 *  A generic, line-oriented sink function pointer. The meaning of 'user'
 *  is defined by the consumer (the party that provides the sink).
 *
 *  Parameters:
 *    user   - opaque pointer supplied by the consumer when wiring the sink.
 *    line   - a UTF-8 string for one logical line of output (may be empty).
 *    is_err - TRUE if the line came from stderr; FALSE for stdout.
 *-------------------------------------------------------------------------*/
typedef void (*UmiOutputSink)(gpointer user, const char *line, gboolean is_err);

G_END_DECLS

#endif /* UMI_OUTPUT_SINK_H */

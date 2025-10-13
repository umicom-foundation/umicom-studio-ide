/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/umi_output_sink.h
 *
 * PURPOSE:
 *   A tiny, *decoupled* interface used by the build subsystem to publish
 *   lines of output (stdout/stderr) and diagnostics to whichever UI or
 *   consumer is interested — without including headers from other modules.
 *
 *   This avoids tight coupling to UI panes (e.g. NOT referencing headers
 *   from any UI-specific folders).  Downstream modules can implement this
 *   interface in any way they like.
 *
 * NOTES:
 *   - No GTK types are used here. Pure GLib only.
 *   - Centralizes diagnostic severity (UmiDiagSeverity) so multiple headers
 *     don’t each re-declare the same enum.
 *
 * THREADING:
 *   - Sinks should assume callbacks may arrive from the GLib main context
 *     used by the build runner. If they need to marshal to another
 *     thread/context they should do so themselves.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_OUTPUT_SINK_H
#define UMI_OUTPUT_SINK_H

#include <glib.h>

G_BEGIN_DECLS

/*---------------------------------------------------------------------------
 * Diagnostic severity
 *---------------------------------------------------------------------------*/
typedef enum
{
    UMI_DIAG_ERROR   = 0,  /* Highest priority: build-breaking / critical        */
    UMI_DIAG_WARNING = 1,  /* Medium priority: suspicious but not fatal          */
    UMI_DIAG_NOTE    = 2   /* Lowest priority: info, notes, hints                */
} UmiDiagSeverity;

/*---------------------------------------------------------------------------
 * UmiOutputSink — a small vtable the build system writes to.
 *
 *  user     : Opaque pointer handed back to callbacks.
 *  on_line  : Called for each complete line from the process (stdout/stderr).
 *  on_diag  : Optional; called when a structured diagnostic is parsed.
 *---------------------------------------------------------------------------*/
typedef struct UmiOutputSink_
{
    gpointer user; /* consumer state */

    void (*on_line)(gpointer user, const char *line, gboolean is_stderr);

    void (*on_diag)(gpointer       user,
                    UmiDiagSeverity sev,
                    const char     *file,    /* may be NULL */
                    int             line,    /* 1-based, or 0 if unknown */
                    const char     *message  /* never NULL, empty OK */
                   );
} UmiOutputSink;

G_END_DECLS

#endif /* UMI_OUTPUT_SINK_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/problem_router.c
 *
 * PURPOSE:
 *   Implementation of the simple “Problem Router”: takes raw lines (usually
 *   from build/run tools) and:
 *     - tries to parse them into diagnostics and push to UmiProblemList, and
 *     - mirrors any line (parsed or not) to an abstract UmiOutputSink.
 *
 * DESIGN:
 *   - Public header is minimal and forward-declares consumer types to keep
 *     coupling low. Here in the .c we include the concrete public headers:
 *       • problem_list.h        (append/clear model)
 *       • diagnostic_parsers.h  (normalize tool output to UmiDiag)
 *       • umi_output_sink.h     (abstract sink for mirroring text/diags)
 *   - Never uses deep/relative include paths — headers are included by name.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "problem_router.h"        /* our own API surface                      */
#include "problem_list.h"          /* umi_problem_list_* API                   */
#include "diagnostic_parsers.h"    /* UmiDiagParser / UmiDiag                  */
#include "umi_output_sink.h"       /* UmiOutputSink + helpers                  */

/* Begin a new routing session:
 * - Clear the Problems model so the user sees only fresh diagnostics.
 * - Emit a small banner via the abstract sink (optional, UX-friendly).      */
void umi_problem_router_begin(UmiProblemRouter *r)
{
  if (!r) return;

  /* Problems model may be absent in headless/CLI scenarios; guard pointers. */
  if (r->plist) {
    (void)umi_problem_list_clear(r->plist);
  }

  /* Output sink is optional; mirror a small “start” marker for context.     */
  if (r->out) {
    umi_output_sink_append_line(r->out, "[problems] started");
  }
}

/* Route a single line:
 * - Try to parse into a UmiDiag (severity, file, line, message…).
 * - If parsed, add it to the Problems model.
 * - Always mirror the raw line to the sink for transparency.                */
void umi_problem_router_feed(UmiProblemRouter *r, const char *line)
{
  if (!r || !line) return;

  /* Mirror first so users see the exact tool output even if parsing fails.  */
  if (r->out) {
    umi_output_sink_append_line(r->out, line);
  }

  /* Try to parse as a diagnostic and push into the Problems list.           */
  if (r->plist) {
    UmiDiagParser *p = umi_diag_parser_new(NULL);
    UmiDiag *diag = NULL;

    if (umi_diag_parser_feed_line(p, line, &diag) && diag) {
      (void)umi_problem_list_add(r->plist, diag);
      umi_diag_free(diag);
    }

    umi_diag_parser_free(p);
  }
}

/* End of session:                                                            */
void umi_problem_router_end(UmiProblemRouter *r)
{
  if (!r) return;

  if (r->out) {
    umi_output_sink_append_line(r->out, "[problems] done");
  }
}
/*  END OF FILE */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/problem_router.c
 *
 * PURPOSE:
 *   Implementation of the simple “Problem Router”: takes raw lines (usually
 *   from build/run tools) and:
 *     - tries to parse them into diagnostics and push to UmiProblemList, and
 *     - mirrors any line (parsed or not) to UmiOutputPane for visibility.
 *
 * DESIGN:
 *   - Public header is minimal and forward-declares consumer types to keep
 *     coupling low. Here in the .c we include the concrete public headers:
 *       • problem_list.h   (append/clear model)
 *       • output_pane.h    (append lines to UI)
 *       • diagnostic_parsers.h (normalize tool output to UmiDiag)
 *   - Never uses deep/relative include paths — headers are included by name.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "problem_router.h"        /* our own API surface                      */
#include "problem_list.h"          /* model operations                         */
#include "output_pane.h"           /* output mirror                            */
#include "diagnostic_parsers.h"    /* UmiDiagParser / UmiDiag                 "
#include "umi_output_sink.h"       /* UmiDiag severity/type if needed          */

/* Begin a new routing session:
 * - Clear the Problems model so the user sees only fresh diagnostics.
 * - Emit a small banner in the Output pane (optional, UX-friendly).         */
void umi_problem_router_begin(UmiProblemRouter *r)
{
  if (!r) return;                                   /* tolerate NULL router            */

  /* Problems model may be absent in headless/CLI scenarios; guard pointers. */
  if (r->plist) {
    /* Clear old diagnostics so counts/lists start from zero.                */
    (void)problem_list_clear(r->plist);             /* returns removed count (unused) */
  }

  /* Output pane is optional; mirror a small “start” marker for context.     */
  if (r->out) {
    output_pane_append_line(r->out, "[problems] started");
  }
}

/* Route a single line:
 * - Try to parse into a UmiDiag (severity, file, line, message…).
 * - If parsed, add it to the Problems model.
 * - Always mirror the raw line to the Output pane for transparency.         */
void umi_problem_router_feed(UmiProblemRouter *r, const char *line)
{
  if (!r || !line) return;                           /* nothing to do                   */

  /* Mirror first so users see the exact tool output even if parsing fails.  */
  if (r->out) {
    output_pane_append_line(r->out, line);
  }

  /* Try to parse as a diagnostic and push into the Problems list.           */
  if (r->plist) {
    UmiDiagParser *p = umi_diag_parser_new(NULL);    /* generic parser (tool auto/heur) */
    UmiDiag *diag = NULL;

    if (umi_diag_parser_feed_line(p, line, &diag) && diag) {
      (void)problem_list_add(r->plist, diag);       /* take a copy into the model      */
      umi_diag_free(diag);                           /* free our temporary copy         */
    }

    umi_diag_parser_free(p);                         /* release parser instance         */
  }
}

/* End of session:
 * - Optionally compute/print a summary later (count errors/warnings/notes).
 * - Emit an “end” banner for Output so the user knows we’re done.           */
void umi_problem_router_end(UmiProblemRouter *r)
{
  if (!r) return;

  if (r->out) {
    output_pane_append_line(r->out, "[problems] done");
  }
}
/*-----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
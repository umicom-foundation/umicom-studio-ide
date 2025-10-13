/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/diagnostics_router.c
 *
 * PURPOSE:
 *   Implementation of the diagnostics router. Maintains a single parser
 *   instance between begin() and end() so repeated feed() calls are fast.
 *
 * DESIGN:
 *   - Includes public headers by name only.
 *   - Carefully guards all pointers (router + sinks).
 *   - Prints a short start/end banner to Output for UX trace.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include "diagnostics_router.h"  /* our struct + API                             */
#include "problem_list.h"        /* problem_list_clear/add                        */
#include "output_pane.h"         /* output_pane_append_line                       */
#include "diagnostic_parsers.h"  /* UmiDiagParser / feed_line                     */
#include "umi_output_sink.h"     /* UmiDiag / severity                            */

/* Start a routing session:
 * - Create internal parser (generic, tool autodetect/heuristic).
 * - Clear Problems model and print a small banner.                            */
void umi_diag_router_begin(UmiDiagRouter *dr)
{
  if (!dr) return;

  /* Reset any previous parser first (defensive programming).                 */
  if (dr->parser) {
    umi_diag_parser_free(dr->parser);
    dr->parser = NULL;
  }

  /* Fresh parser — leave tool_name NULL for generic heuristics.              */
  dr->parser = umi_diag_parser_new(NULL);

  if (dr->plist) (void)problem_list_clear(dr->plist);   /* remove prior diagnostics */
  if (dr->out)   output_pane_append_line(dr->out, "[diag] parsing started");
}

/* Feed a raw line:
 * - Always mirror to Output.
 * - If parser recognizes a diagnostic, add it to Problems.                   */
void umi_diag_router_feed(UmiDiagRouter *dr, const char *line)
{
  if (!dr || !line) return;

  if (dr->out) output_pane_append_line(dr->out, line);

  if (dr->plist && dr->parser) {
    UmiDiag *d = NULL;
    if (umi_diag_parser_feed_line(dr->parser, line, &d) && d) {
      (void)problem_list_add(dr->plist, d);       /* store a copy into the model */
      umi_diag_free(d);                            /* free our temporary copy      */
    }
  }
}

/* End a routing session:
 * - Free internal parser.
 * - Print a closing banner; counters can be added later.                     */
void umi_diag_router_end(UmiDiagRouter *dr)
{
  if (!dr) return;

  if (dr->parser) {
    umi_diag_parser_free(dr->parser);
    dr->parser = NULL;
  }

  if (dr->out) output_pane_append_line(dr->out, "[diag] parsing done");
}

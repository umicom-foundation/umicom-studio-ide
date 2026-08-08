/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/diagnostics_router.h
 *
 * PURPOSE:
 *   Higher-level diagnostics router that (a) mirrors every line to the
 *   Output pane and (b) parses diagnostics using UmiDiagParser and adds them
 *   to the Problems list. This is like ProblemRouter but bundles a persistent
 *   parser instance between begin()/end() for efficiency.
 *
 * DESIGN:
 *   - Public header declares a small struct with pointers to sinks and an
 *     internal parser pointer. Call begin() to initialize, then feed() lines,
 *     then end() to finalize/summary.
 *   - Includes only public headers by name; no relative paths.
 *
 * API:
 *   typedef struct UmiDiagRouter UmiDiagRouter;
 *   void umi_diag_router_begin(UmiDiagRouter *dr);
 *   void umi_diag_router_feed (UmiDiagRouter *dr, const char *line);
 *   void umi_diag_router_end  (UmiDiagRouter *dr);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_ROUTER_H
#define UMICOM_DIAGNOSTICS_ROUTER_H

/* Forward declarations keep coupling low in the header. The .c will include
 * the full headers (problem_list.h, output_pane.h, diagnostic_parsers.h).    */
typedef struct _UmiProblemList UmiProblemList;
typedef struct _UmiOutputPane  UmiOutputPane;
typedef struct _UmiDiagParser  UmiDiagParser;

/* Router state: sinks + internal parser instance (opaque outside).          */
typedef struct {
  UmiProblemList *plist;      /* destination for normalized diagnostics       */
  UmiOutputPane  *out;        /* destination for verbatim output lines        */
  UmiDiagParser  *parser;     /* internal parser kept across feed() calls     */
  /* Future: counters for summary (errors/warnings/notes) can live here.     */
} UmiDiagRouter;

void umi_diag_router_begin(UmiDiagRouter *dr);             /* init + clear    */
void umi_diag_router_feed (UmiDiagRouter *dr, const char *line); /* route one */
void umi_diag_router_end  (UmiDiagRouter *dr);             /* finalize/report */

#endif /* UMICOM_DIAGNOSTICS_ROUTER_H */

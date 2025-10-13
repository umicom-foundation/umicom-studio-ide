/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/problem_router.h
 *
 * PURPOSE:
 *   Glue API for routing raw text lines from tools (build/run/test output)
 *   into two destinations:
 *     1) the Problems model (normalized diagnostics), and
 *     2) the Output pane (verbatim mirror).
 *
 * DESIGN:
 *   - This header intentionally avoids pulling heavy UI headers. We forward
 *     declare the consumer types (UmiProblemList, UmiOutputPane) because we
 *     only keep *pointers* to them here (no dereference in the header).
 *   - The .c file includes the real public headers that define the functions.
 *   - Call order is always: begin() -> feed()...feed() -> end().
 *
 * API:
 *   typedef struct UmiProblemRouter UmiProblemRouter;
 *   void umi_problem_router_begin(UmiProblemRouter *r);
 *   void umi_problem_router_feed (UmiProblemRouter *r, const char *line);
 *   void umi_problem_router_end  (UmiProblemRouter *r);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROBLEM_ROUTER_H
#define UMICOM_PROBLEM_ROUTER_H

/* Forward declarations only — keep coupling low in public header.
 * The actual definitions live in the Problems and Output modules.          */
typedef struct _UmiProblemList UmiProblemList;  /* problems pane model (opaque) */
typedef struct _UmiOutputPane  UmiOutputPane;   /* output console/pane (opaque) */

/* Opaque router object for this simple glue (kept as a struct here because
 * it only holds pointers; implementation details are in the .c file).       */
typedef struct {
  UmiProblemList *plist; /* destination for normalized diagnostics          */
  UmiOutputPane  *out;   /* destination for verbatim lines                  */
} UmiProblemRouter;

/* Clear Problems list and optionally print a “start” banner to Output.     */
void umi_problem_router_begin(UmiProblemRouter *r);

/* Feed one raw line; if parsers don’t recognize it, mirror to Output.      */
void umi_problem_router_feed (UmiProblemRouter *r, const char *line);

/* Optionally print a summary and an “end” banner to Output.                */
void umi_problem_router_end  (UmiProblemRouter *r);

#endif /* UMICOM_PROBLEM_ROUTER_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/problem_router.h
 *
 * PURPOSE:
 *   Small coordinator that resets the problems list at the start of a run,
 *   feeds build output lines into the parser, and emits basic notifications
 *   to the output sink.
 *
 * DESIGN:
 *   - No UI types here; uses the abstract UmiOutputSink.
 *   - Holds a pointer to the shared UmiProblemList model (owned elsewhere).
 *
 * API:
 *   typedef struct UmiProblemRouter UmiProblemRouter;
 *
 *   void umi_problem_router_begin(UmiProblemRouter *r);
 *   void umi_problem_router_feed (UmiProblemRouter *r, const char *line_utf8);
 *   void umi_problem_router_end  (UmiProblemRouter *r);
 *
 *   // Constructor/Destructor
 *   UmiProblemRouter* umi_problem_router_new(UmiProblemList *list, UmiOutputSink *sink);
 *   void              umi_problem_router_free(UmiProblemRouter *r);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_PROBLEM_ROUTER_H
#define UMI_PROBLEM_ROUTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "umi_output_sink.h"  /* <- abstract sink */
#include "problem_list.h"     /* <- list + parser entrypoint */

/* Minimal coordinator that glues parsing with model + sink. */
typedef struct UmiProblemRouter {
    UmiProblemList *plist;  /* not owned */
    UmiOutputSink  *out;    /* not owned */
} UmiProblemRouter;

/* Lifecycle */
UmiProblemRouter* umi_problem_router_new (UmiProblemList *list, UmiOutputSink *sink);
void              umi_problem_router_free(UmiProblemRouter *r);

/* Flow */
void umi_problem_router_begin(UmiProblemRouter *r);
void umi_problem_router_feed (UmiProblemRouter *r, const char *line_utf8);
void umi_problem_router_end  (UmiProblemRouter *r);

#ifdef __cplusplus
}
#endif
#endif /* UMI_PROBLEM_ROUTER_H */

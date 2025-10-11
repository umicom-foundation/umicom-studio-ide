/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/diagnostics_router.h
 * PURPOSE: Route process lines to problem_list and output pane
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_ROUTER_H
#define UMICOM_DIAGNOSTICS_ROUTER_H

#include "../../panes/problems/include/problem_list.h"
#include "../../panes/output/include/output_pane.h"

typedef struct {
  UmiProblemList *plist;
  UmiOutputPane  *out;
} UmiDiagRouter;

/* Feed a single line from tools; parse known formats and mirror to console. */
void umi_diag_router_feed(UmiDiagRouter *dr, const char *line);

/* Clear problem list at the start of a run. */
void umi_diag_router_begin(UmiDiagRouter *dr);

/* Summarize at end of a run (counts). */
void umi_diag_router_end(UmiDiagRouter *dr);

#endif /* UMICOM_DIAGNOSTICS_ROUTER_H */

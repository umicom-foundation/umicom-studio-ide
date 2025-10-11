/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/problem_router.h
 * PURPOSE: Glue: feed lines to parsers and update list + output
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROBLEM_ROUTER_H
#define UMICOM_PROBLEM_ROUTER_H

#include "problem_list.h"
#include "output_pane.h"

typedef struct {
  UmiProblemList *plist;
  UmiOutputPane  *out;
} UmiProblemRouter;

void umi_problem_router_begin(UmiProblemRouter *r);
void umi_problem_router_feed (UmiProblemRouter *r, const char *line);
void umi_problem_router_end  (UmiProblemRouter *r);

#endif /* UMICOM_PROBLEM_ROUTER_H */

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/problem_router.c
 * PURPOSE: Implementation of problem router using diagnostic_parsers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/problem_router.h"

void umi_problem_router_begin(UmiProblemRouter *r){
  if(!r) return;
  umi_problem_list_clear(r->plist);
  if(r->out) umi_output_pane_append_line(r->out, "[problems] started");
}

void umi_problem_router_feed(UmiProblemRouter *r, const char *line){
  if(!r || !line) return;
  if(!umi_problem_parse_any(r->plist, line)){
    if(r->out) umi_output_pane_append_line(r->out, line);
  }
}

void umi_problem_router_end(UmiProblemRouter *r){
  if(!r) return;
  if(r->out) umi_output_pane_append_line(r->out, "[problems] done");
}

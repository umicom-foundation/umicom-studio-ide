/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/diagnostics_router.c
 * PURPOSE: Implementation of diagnostics routing
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "diagnostics_router.h"

void umi_diag_router_begin(UmiDiagRouter *dr){
  if(!dr) return;
  umi_problem_list_clear(dr->plist);
  umi_output_pane_append_line(dr->out, "[diag] parsing started");
}

void umi_diag_router_feed(UmiDiagRouter *dr, const char *line){
  if(!dr || !line) return;
  if(!umi_problem_parse_any(dr->plist, line)){
    /* Mirror unparsed lines to console */
    umi_output_pane_append_line(dr->out, line);
  }
}

void umi_diag_router_end(UmiDiagRouter *dr){
  (void)dr;
  /* Future: count and print summary */
}

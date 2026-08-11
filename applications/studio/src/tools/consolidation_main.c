/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/consolidation_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for CodeGuard security and consolidation
 *   inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include "umicom/studio/consolidation.h"
#include <stdio.h>
int main(int argc,char **argv){UmiStudioConsolidationReport r;const char *root=argc>1?argv[1]:".";if(umi_studio_consolidation_check(root,&r)!=UMI_STATUS_OK)return 2;printf("Studio consolidation: architecture=%zu duplicates=%zu high-risk=%zu ready=%s\n",r.architecture_findings,r.duplicates,r.high_risk,r.ready?"yes":"no");return r.ready?0:1;}

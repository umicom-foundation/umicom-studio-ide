/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/quality_main.c
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
#include "umicom/studio/quality_gate.h"
#include <stdio.h>
int main(int argc,char **argv){int passed=0;UmiCodeGuardSummary s;const char *root=argc>1?argv[1]:".";if(umi_studio_quality_gate(root,&passed,&s)!=UMI_STATUS_OK)return 2;printf("Code quality gate: %s (critical=%zu high=%zu architecture=%zu duplicates=%zu)\n",passed?"PASS":"FAIL",s.critical,s.high,s.architecture,s.duplicates);return passed?0:1;}

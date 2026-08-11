/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/duplicates_main.c
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
#include "umicom/studio/duplicate_review.h"
#include <stdio.h>
int main(int argc,char **argv){size_t n=0U;const char *root=argc>1?argv[1]:".";if(umi_studio_duplicate_review(root,&n)!=UMI_STATUS_OK)return 2;printf("Duplicate source findings: %zu\n",n);return n==0U?0:1;}

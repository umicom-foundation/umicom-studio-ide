/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/quality_gate.c
 *
 * PURPOSE:
 *   Integrate Framework CodeGuard analysis into Studio security and
 *   consolidation workflows.
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
UmiStatus umi_studio_quality_gate(const char *root,int *passed,UmiCodeGuardSummary *summary){UmiStudioCodeGuard *g=NULL;UmiCodeGuardProfile p=umi_codeguard_profile_ci();UmiStatus s;if(root==NULL||passed==NULL||summary==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_studio_codeguard_create(root,&g);if(s==UMI_STATUS_OK)s=umi_studio_codeguard_scan(g,&p);if(s==UMI_STATUS_OK){*summary=umi_studio_codeguard_summary(g);*passed=!umi_codeguard_summary_failed(summary,p.fail_on)&&summary->architecture==0U;}umi_studio_codeguard_destroy(g);return s;}

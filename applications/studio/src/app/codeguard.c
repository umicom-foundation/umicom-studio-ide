/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/codeguard.c
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
#include "umicom/studio/codeguard.h"
#include <stdlib.h>
struct UmiStudioCodeGuard { UmiCodeGuardService *service; };
UmiStatus umi_studio_codeguard_create(const char *root,UmiStudioCodeGuard **out){UmiStudioCodeGuard *g;UmiStatus s;if(root==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;g=(UmiStudioCodeGuard*)calloc(1U,sizeof(*g));if(g==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_codeguard_service_create(root,&g->service);if(s!=UMI_STATUS_OK){free(g);return s;}*out=g;return UMI_STATUS_OK;}
void umi_studio_codeguard_destroy(UmiStudioCodeGuard *g){if(g!=NULL){umi_codeguard_service_destroy(g->service);free(g);}}
UmiStatus umi_studio_codeguard_scan(UmiStudioCodeGuard *g,const UmiCodeGuardProfile *p){return g!=NULL?umi_codeguard_service_scan(g->service,p):UMI_STATUS_INVALID_ARGUMENT;}
const UmiCodeGuardResult *umi_studio_codeguard_result(const UmiStudioCodeGuard *g){return g!=NULL?umi_codeguard_service_result(g->service):NULL;}
UmiCodeGuardSummary umi_studio_codeguard_summary(const UmiStudioCodeGuard *g){return g!=NULL?umi_codeguard_service_summary(g->service):(UmiCodeGuardSummary){0};}

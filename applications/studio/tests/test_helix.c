/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_helix.c
 *
 * PURPOSE:
 *   Validate Studio's Framework-backed AI and Helix integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test verifies one product adapter while keeping the provider-neutral implementation inside Framework.
 */



#include <assert.h>
#include <string.h>
#include "umicom/studio/helix.h"
int main(void){UmiStudioAiPlatform *p=NULL;char out[256];assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_helix_status(p,out,sizeof(out))==UMI_STATUS_OK);assert(strstr(out,"agents=6")!=NULL);assert(umi_studio_helix_advance(p,UMI_HELIX_STAGE_DIAGNOSE)==UMI_STATUS_OK);umi_studio_ai_platform_destroy(p);return 0;}

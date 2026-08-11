/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_helix_evidence.c
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
#include "umicom/studio/helix_evidence.h"
int main(void){UmiStudioAiPlatform *p=NULL;assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_helix_record_evidence(p,"tests","passed")==UMI_STATUS_OK);assert(strcmp(umi_studio_helix_evidence(p,"tests"),"passed")==0);umi_studio_ai_platform_destroy(p);return 0;}

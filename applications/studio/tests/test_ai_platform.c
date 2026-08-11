/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ai_platform.c
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
#include "umicom/studio/ai_platform.h"
int main(void){UmiStudioAiPlatform *p=NULL;assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(p!=NULL);assert(umi_studio_ai_platform_runtime(p)!=NULL);assert(umi_studio_ai_platform_helix(p)!=NULL);umi_studio_ai_platform_destroy(p);return 0;}

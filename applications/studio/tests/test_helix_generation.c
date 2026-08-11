/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_helix_generation.c
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
#include "umicom/studio/helix_generation.h"
#include "umicom/studio/helix_fitness.h"
int main(void){UmiStudioAiPlatform *p=NULL;UmiHelixFitness f;assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_helix_fitness(0.95,0.95,0.9,0.9,0.95,&f)==UMI_STATUS_OK);umi_studio_ai_platform_helix(p)->policy.allow_release=1;assert(umi_studio_helix_can_promote(p,&f,3U,3U,1,1)==UMI_STATUS_OK);umi_studio_ai_platform_destroy(p);return 0;}

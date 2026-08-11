/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_helix_fitness.c
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
#include "umicom/studio/helix_fitness.h"
int main(void){UmiHelixFitness f;assert(umi_studio_helix_fitness(0.95,0.95,0.9,0.9,0.95,&f)==UMI_STATUS_OK);assert(f.passed);return 0;}

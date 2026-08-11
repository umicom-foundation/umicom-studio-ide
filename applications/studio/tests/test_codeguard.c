/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_codeguard.c
 *
 * PURPOSE:
 *   Validate Studio integration with Framework CodeGuard and consolidation
 *   services.
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
#include <assert.h>
#include "umicom/studio/codeguard.h"
int main(void){UmiStudioCodeGuard *g=NULL;assert(umi_studio_codeguard_create(".",&g)==UMI_STATUS_OK);assert(g!=NULL);umi_studio_codeguard_destroy(g);return 0;}

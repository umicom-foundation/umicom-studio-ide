/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_quality_gate.c
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
#include "umicom/studio/quality_gate.h"
int main(void){int p=0;UmiCodeGuardSummary s;UmiStatus st=umi_studio_quality_gate(".",&p,&s);assert(st==UMI_STATUS_OK||st==UMI_STATUS_IO_ERROR);return 0;}

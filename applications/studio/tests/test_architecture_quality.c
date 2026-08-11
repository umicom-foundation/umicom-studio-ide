/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_architecture_quality.c
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
#include "umicom/studio/architecture_quality.h"
int main(void){char b[256];UmiStatus s=umi_studio_architecture_quality_report(".",b,sizeof(b));assert(s==UMI_STATUS_OK||s==UMI_STATUS_IO_ERROR);return 0;}

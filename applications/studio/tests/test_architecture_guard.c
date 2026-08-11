/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_architecture_guard.c
 *
 * PURPOSE:
 *   Validate architecture guard behaviour in the reference IDE consumer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This integration test proves Studio consumes the public Framework contract rather than a private duplicate implementation.
 */
#include <assert.h>
#include "umicom/studio/architecture_guard.h"
int main(void){UmiStudioArchitectureGuard r;assert(umi_studio_architecture_guard_run(&r)==UMI_STATUS_OK);assert(r.profiles_checked==5U);return 0;}

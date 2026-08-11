/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_conformance_platform.c
 *
 * PURPOSE:
 *   Validate conformance platform behaviour in the reference IDE consumer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This integration test proves Studio consumes the public Framework contract rather than a private duplicate implementation.
 */
#include <assert.h>
#include "umicom/studio/conformance.h"
int main(void){UmiStudioCompatibilityPlatform *p=NULL;UmiStudioConformanceResult r;assert(umi_studio_compatibility_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_conformance_run(p,&r)==UMI_STATUS_OK);assert(r.products_checked==5U);umi_studio_compatibility_platform_destroy(p);return 0;}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_compatibility_platform.c
 *
 * PURPOSE:
 *   Validate compatibility platform behaviour in the reference IDE consumer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This integration test proves Studio consumes the public Framework contract rather than a private duplicate implementation.
 */
#include <assert.h>
#include "umicom/studio/compatibility_platform.h"
int main(void){UmiStudioCompatibilityPlatform *p=NULL;assert(umi_studio_compatibility_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_compatibility_product_count(p)==5U);umi_studio_compatibility_platform_destroy(p);return 0;}

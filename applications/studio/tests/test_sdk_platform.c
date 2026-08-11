/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_sdk_platform.c
 *
 * PURPOSE:
 *   Validate sdk platform behaviour in the reference IDE consumer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This integration test proves Studio consumes the public Framework contract rather than a private duplicate implementation.
 */
#include <assert.h>
#include "umicom/studio/sdk.h"
int main(void){UmiSdkRequirement r={{0U,5U,0U},UMICOM_FRAMEWORK_ABI_VERSION};UmiSdkManifest m=umi_sdk_manifest_current(NULL,0U);assert(umi_sdk_compatible(&r,&m));return 0;}

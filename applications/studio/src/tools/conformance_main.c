/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/conformance_main.c
 *
 * PURPOSE:
 *   Run major-release conformance checks from Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command exposes one compatibility check in a form that is easy to run from PowerShell, CI, or the future Studio workbench UI.
 */
#include <stdio.h>
#include "umicom/studio/conformance.h"
int main(void){UmiStudioCompatibilityPlatform *p=NULL;UmiStudioConformanceResult r;if(umi_studio_compatibility_platform_create(&p)!=UMI_STATUS_OK)return 1;(void)umi_studio_conformance_run(p,&r);printf("Products: %zu/%zu compatible\nABI: %s\nConformance: %s\n",r.products_compatible,r.products_checked,r.abi_ok?"PASS":"FAIL",r.passed?"PASS":"FAIL");umi_studio_compatibility_platform_destroy(p);return r.passed?0:2;}

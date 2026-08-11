/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ai_tools.c
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
#include <string.h>
#include "umicom/studio/ai_tools.h"
int main(void){UmiStudioAiPlatform *p=NULL;char out[64];assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_ai_tools_register_defaults(p)==UMI_STATUS_OK);assert(umi_studio_ai_tools_echo(p,"{}",0,out,sizeof(out))==UMI_STATUS_PERMISSION_DENIED);assert(umi_studio_ai_tools_echo(p,"{}",1,out,sizeof(out))==UMI_STATUS_OK);assert(strstr(out,"echo:")!=NULL);umi_studio_ai_platform_destroy(p);return 0;}

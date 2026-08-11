/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ai_chat.c
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
#include "umicom/studio/ai_chat.h"
int main(void){UmiStudioAiPlatform *p=NULL;UmiAiResponse r;assert(umi_studio_ai_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_ai_chat(p,"hello",&r)==UMI_STATUS_OK);assert(strstr(r.text,"hello")!=NULL);umi_studio_ai_platform_destroy(p);return 0;}

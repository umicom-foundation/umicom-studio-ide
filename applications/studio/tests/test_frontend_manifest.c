/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_frontend_manifest.c
 *
 * PURPOSE:
 *   Verify Studio integration with Framework web, browser, charting and frontend services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test uses public Studio and Framework contracts so it documents the expected behaviour for new contributors.
 */

#include "umicom/studio/web_platform.h"
#include "umicom/studio/frontend.h"
#include <assert.h>
#include <string.h>
int main(void){UmiStudioWebPlatform *p=NULL;char out[2048];assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_reset(p,UMI_FRONTEND_KIND_WEB)==UMI_STATUS_OK);assert(umi_studio_frontend_generate_manifest(p,out,sizeof(out))==UMI_STATUS_OK);assert(strstr(out,"web")!=NULL);umi_studio_web_platform_destroy(p);return 0;}

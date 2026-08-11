/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_web.c
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
#include "umicom/studio/web.h"
#include <assert.h>
#include <string.h>
int main(void){UmiStudioWebPlatform *p=NULL;char out[4096];assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_web_process(p,"GET /api/health HTTP/1.1\r\nHost: x\r\n\r\n",out,sizeof(out))==UMI_STATUS_OK);assert(strstr(out,"ready")!=NULL);umi_studio_web_platform_destroy(p);return 0;}

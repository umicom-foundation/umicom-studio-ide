/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_browser.c
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
#include "umicom/studio/browser.h"
#include <assert.h>
#include <string.h>
int main(void){UmiStudioWebPlatform *p=NULL;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_browser_open(p,"https://umicom.foundation")==UMI_STATUS_OK);assert(strcmp(umi_studio_browser_current(p),"https://umicom.foundation")==0);umi_studio_web_platform_destroy(p);return 0;}

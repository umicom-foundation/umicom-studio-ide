/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/web_platform_main.c
 *
 * PURPOSE:
 *   Provide a small native command for the Studio Web Server and multi-frontend platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command is intentionally small: it calls the same Studio service used by GUI code, which makes the feature easy to test from a terminal.
 */

#include "umicom/studio/web_platform.h"
#include "umicom/studio/browser.h"
#include "umicom/studio/charting.h"
#include "umicom/studio/frontend.h"
#include <stdio.h>
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioWebPlatformSnapshot snap;UmiStatus s=umi_studio_web_platform_create(&p);if(s==UMI_STATUS_OK)s=umi_studio_browser_open(p,"https://umicom.foundation");if(s==UMI_STATUS_OK)s=umi_studio_charting_add_close(p,1,100.0);if(s==UMI_STATUS_OK)s=umi_studio_frontend_reset(p,UMI_FRONTEND_KIND_WEB);if(s==UMI_STATUS_OK)s=umi_studio_web_platform_snapshot(p,&snap);if(s==UMI_STATUS_OK)(void)printf("Routes: %zu\nEndpoints: %zu\nChart series: %zu\nFrontend pages: %zu\nBrowser: %s\n",snap.routes,snap.endpoints,snap.chart_series,snap.frontend_pages,snap.browser_url);umi_studio_web_platform_destroy(p);return s==UMI_STATUS_OK?0:1;}

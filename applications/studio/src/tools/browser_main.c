/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/browser_main.c
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
#include <stdio.h>
int main(void){UmiStudioWebPlatform *p=NULL;UmiStatus s=umi_studio_web_platform_create(&p);if(s==UMI_STATUS_OK)s=umi_studio_browser_open(p,"https://umicom.foundation");if(s==UMI_STATUS_OK)(void)printf("Browser URL: %s\n",umi_studio_browser_current(p));umi_studio_web_platform_destroy(p);return s==UMI_STATUS_OK?0:1;}

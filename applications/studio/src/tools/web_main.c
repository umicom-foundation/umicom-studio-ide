/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/web_main.c
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
#include "umicom/studio/web.h"
#include <stdio.h>
int main(void){UmiStudioWebPlatform *p=NULL;char response[4096];UmiStatus s=umi_studio_web_platform_create(&p);if(s==UMI_STATUS_OK)s=umi_studio_web_process(p,"GET /api/health HTTP/1.1\r\nHost: local\r\n\r\n",response,sizeof(response));if(s==UMI_STATUS_OK)(void)puts(response);umi_studio_web_platform_destroy(p);return s==UMI_STATUS_OK?0:1;}

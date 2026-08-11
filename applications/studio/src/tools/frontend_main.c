/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/frontend_main.c
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
#include "umicom/studio/frontend.h"
#include <stdio.h>
int main(void){UmiStudioWebPlatform *p=NULL;char html[2048],manifest[1024];UmiStatus s=umi_studio_web_platform_create(&p);if(s==UMI_STATUS_OK)s=umi_studio_frontend_reset(p,UMI_FRONTEND_KIND_WEB);if(s==UMI_STATUS_OK)s=umi_studio_frontend_generate_html(p,html,sizeof(html));if(s==UMI_STATUS_OK)s=umi_studio_frontend_generate_manifest(p,manifest,sizeof(manifest));if(s==UMI_STATUS_OK){(void)puts(html);(void)puts(manifest);}umi_studio_web_platform_destroy(p);return s==UMI_STATUS_OK?0:1;}

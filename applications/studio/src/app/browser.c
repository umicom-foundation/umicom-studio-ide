/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/browser.c
 *
 * PURPOSE:
 *   Implement Studio embedded-browser navigation through the Framework browser function table.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio asks the browser contract to navigate. A future WebKit or Edge adapter can replace the headless provider without changing this use case.
 */

#include "umicom/studio/browser.h"
UmiStatus umi_studio_browser_open(UmiStudioWebPlatform *p,const char *url){UmiBrowser *b;if(p==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;b=umi_studio_web_platform_browser(p);return b!=NULL&&b->navigate!=NULL?b->navigate(b->instance,url):UMI_STATUS_INVALID_STATE;}
const char *umi_studio_browser_current(const UmiStudioWebPlatform *p){UmiBrowser *b=umi_studio_web_platform_browser((UmiStudioWebPlatform*)p);return b!=NULL&&b->current_url!=NULL?b->current_url(b->instance):NULL;}

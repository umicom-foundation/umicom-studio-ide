/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/browser.h
 *
 * PURPOSE:
 *   Expose Studio navigation use cases over the Framework browser contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a thin Studio-facing API. Reusable HTTP, browser, chart and frontend logic remains in Umicom Framework.
 */

#ifndef UMICOM_STUDIO_BROWSER_H
#define UMICOM_STUDIO_BROWSER_H
#include "umicom/studio/web_platform.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_browser_open(UmiStudioWebPlatform *platform,const char *url);
const char *umi_studio_browser_current(const UmiStudioWebPlatform *platform);
#ifdef __cplusplus
}
#endif
#endif

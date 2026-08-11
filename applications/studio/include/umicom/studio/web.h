/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/web.h
 *
 * PURPOSE:
 *   Expose Studio HTTP processing and default REST endpoint registration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a thin Studio-facing API. Reusable HTTP, browser, chart and frontend logic remains in Umicom Framework.
 */

#ifndef UMICOM_STUDIO_WEB_H
#define UMICOM_STUDIO_WEB_H
#include "umicom/studio/web_platform.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_web_process(UmiStudioWebPlatform *platform,const char *request,char *out_response,size_t capacity);
UmiStatus umi_studio_web_register_defaults(UmiStudioWebPlatform *platform);
#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/frontend.h
 *
 * PURPOSE:
 *   Expose Studio multi-frontend planning and browser-artifact generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a thin Studio-facing API. Reusable HTTP, browser, chart and frontend logic remains in Umicom Framework.
 */

#ifndef UMICOM_STUDIO_FRONTEND_H
#define UMICOM_STUDIO_FRONTEND_H
#include "umicom/studio/web_platform.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_frontend_reset(UmiStudioWebPlatform *platform,UmiFrontendKind kind);
UmiStatus umi_studio_frontend_generate_html(UmiStudioWebPlatform *platform,char *out_text,size_t capacity);
UmiStatus umi_studio_frontend_generate_manifest(UmiStudioWebPlatform *platform,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif

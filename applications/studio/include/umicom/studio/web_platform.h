/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/web_platform.h
 *
 * PURPOSE:
 *   Expose Studio's shared composition of Framework web, browser, charting and frontend services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a thin Studio-facing API. Reusable HTTP, browser, chart and frontend logic remains in Umicom Framework.
 */

#ifndef UMICOM_STUDIO_WEB_PLATFORM_H
#define UMICOM_STUDIO_WEB_PLATFORM_H
#include "umicom/umicom.h"
#include "umicom/frontend/frontend_studio.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioWebPlatform UmiStudioWebPlatform;
typedef struct UmiStudioWebPlatformSnapshot { size_t routes; size_t endpoints; size_t sessions; uint64_t requests; size_t chart_series; size_t frontend_pages; char browser_url[UMI_BROWSER_URL_CAPACITY]; } UmiStudioWebPlatformSnapshot;
UmiStatus umi_studio_web_platform_create(UmiStudioWebPlatform **out_platform);
void umi_studio_web_platform_destroy(UmiStudioWebPlatform *platform);
UmiStatus umi_studio_web_platform_snapshot(const UmiStudioWebPlatform *platform,UmiStudioWebPlatformSnapshot *out_snapshot);
UmiWebService *umi_studio_web_platform_web(UmiStudioWebPlatform *platform);
UmiBrowser *umi_studio_web_platform_browser(UmiStudioWebPlatform *platform);
UmiChartModel *umi_studio_web_platform_chart(UmiStudioWebPlatform *platform);
UmiFrontendPlan *umi_studio_web_platform_frontend(UmiStudioWebPlatform *platform);
UmiFrontendStudio *umi_studio_web_platform_frontend_studio(UmiStudioWebPlatform *platform);
#ifdef __cplusplus
}
#endif
#endif

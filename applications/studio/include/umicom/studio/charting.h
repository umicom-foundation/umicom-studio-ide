/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/charting.h
 *
 * PURPOSE:
 *   Expose Studio chart-model use cases without introducing toolkit-specific chart state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a thin Studio-facing API. Reusable HTTP, browser, chart and frontend logic remains in Umicom Framework.
 */

#ifndef UMICOM_STUDIO_CHARTING_H
#define UMICOM_STUDIO_CHARTING_H
#include "umicom/studio/web_platform.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_charting_reset(UmiStudioWebPlatform *platform,const char *title);
UmiStatus umi_studio_charting_add_close(UmiStudioWebPlatform *platform,int64_t time_ms,double close);
UmiStatus umi_studio_charting_add_sma(UmiStudioWebPlatform *platform,size_t period);
#ifdef __cplusplus
}
#endif
#endif

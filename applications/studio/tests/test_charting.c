/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_charting.c
 *
 * PURPOSE:
 *   Verify Studio integration with Framework web, browser, charting and frontend services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test uses public Studio and Framework contracts so it documents the expected behaviour for new contributors.
 */

#include "umicom/studio/web_platform.h"
#include "umicom/studio/charting.h"
#include <assert.h>
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioWebPlatformSnapshot s;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_charting_add_close(p,1,10.0)==UMI_STATUS_OK);assert(umi_studio_charting_add_close(p,2,11.0)==UMI_STATUS_OK);assert(umi_studio_web_platform_snapshot(p,&s)==UMI_STATUS_OK);assert(s.chart_series==1U);umi_studio_web_platform_destroy(p);return 0;}

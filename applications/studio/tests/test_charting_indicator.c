/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_charting_indicator.c
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
int main(void){UmiStudioWebPlatform *p=NULL;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);for(int i=0;i<5;++i)assert(umi_studio_charting_add_close(p,i,(double)i)==UMI_STATUS_OK);assert(umi_studio_charting_add_sma(p,3U)==UMI_STATUS_OK);umi_studio_web_platform_destroy(p);return 0;}

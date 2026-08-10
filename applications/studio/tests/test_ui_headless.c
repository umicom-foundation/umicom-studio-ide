/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui_headless.c
 *
 * PURPOSE:
 *   Validate the Studio Framework UI composition and workbench integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio/bootstrap.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiUiHeadlessSnapshot s;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);assert(umi_studio_ui_render_headless(umi_studio_bootstrap_ui(b),&s)==UMI_STATUS_OK);assert(strstr(s.text,"Umicom Studio IDE")!=NULL);assert(s.pane_count>=8U);umi_studio_bootstrap_destroy(b);return 0;}

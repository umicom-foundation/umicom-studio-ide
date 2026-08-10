/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui.c
 *
 * PURPOSE:
 *   Validate the Studio Framework UI composition and workbench integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiStudioUiSnapshot s;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);assert(umi_studio_bootstrap_start(b)==UMI_STATUS_OK);assert(umi_studio_ui_snapshot(umi_studio_bootstrap_ui(b),&s)==UMI_STATUS_OK);assert(s.perspectives==8U);assert(s.panes>=8U);assert(s.actions>=8U);umi_studio_bootstrap_destroy(b);return 0;}

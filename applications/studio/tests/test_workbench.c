/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench.c
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
#include "umicom/studio/perspectives.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiUiWorkbenchSnapshot s;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);assert(umi_ui_workbench_activate_perspective(umi_studio_ui_workbench(umi_studio_bootstrap_ui(b)),UMI_STUDIO_PERSPECTIVE_BUILD)==UMI_STATUS_OK);assert(umi_ui_workbench_snapshot(umi_studio_ui_workbench(umi_studio_bootstrap_ui(b)),&s)==UMI_STATUS_OK);assert(strcmp(s.active_perspective,UMI_STUDIO_PERSPECTIVE_BUILD)==0);umi_studio_bootstrap_destroy(b);return 0;}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui_session.c
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
#include "umicom/studio/workbench.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiUiWorkbenchSnapshot s;UmiStudioUi*ui;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);ui=umi_studio_bootstrap_ui(b);assert(umi_ui_workbench_activate_perspective(umi_studio_ui_workbench(ui),UMI_STUDIO_PERSPECTIVE_TEST)==UMI_STATUS_OK);assert(umi_studio_workbench_save_session(umi_studio_ui_workbench(ui),umi_studio_services_session(umi_studio_ui_services(ui)))==UMI_STATUS_OK);assert(umi_studio_workbench_restore_session(umi_studio_ui_workbench(ui),umi_studio_services_session(umi_studio_ui_services(ui)))==UMI_STATUS_OK);assert(umi_ui_workbench_snapshot(umi_studio_ui_workbench(ui),&s)==UMI_STATUS_OK);assert(strcmp(s.active_perspective,UMI_STUDIO_PERSPECTIVE_TEST)==0);umi_studio_bootstrap_destroy(b);return 0;}

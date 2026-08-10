/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui_actions.c
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
#include "umicom/studio/contributions.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiUiPaneSnapshot p;char message[256];UmiUiWorkbench*w;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);w=umi_studio_ui_workbench(umi_studio_bootstrap_ui(b));assert(umi_ui_workbench_execute_action(w,"studio.action.pane.explorer",UMI_STUDIO_PANE_EXPLORER,message,sizeof(message))==UMI_STATUS_OK);assert(umi_ui_pane_model_find(umi_ui_workbench_panes(w),UMI_STUDIO_PANE_EXPLORER,&p)==UMI_STATUS_OK);assert(!p.visible);assert(strstr(message,"hidden")!=NULL);umi_studio_bootstrap_destroy(b);return 0;}

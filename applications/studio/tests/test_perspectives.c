/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_perspectives.c
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
#include "umicom/studio/perspectives.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiUiPerspectiveSnapshot p;UmiUiPerspectiveModel*m;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);m=umi_ui_workbench_perspectives(umi_studio_ui_workbench(umi_studio_bootstrap_ui(b)));assert(umi_ui_perspective_model_count(m)==umi_studio_perspective_definition_count());assert(umi_ui_perspective_model_find(m,UMI_STUDIO_PERSPECTIVE_DEVELOP,&p)==UMI_STATUS_OK);assert(p.active);umi_studio_bootstrap_destroy(b);return 0;}

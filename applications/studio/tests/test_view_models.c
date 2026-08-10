/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_view_models.c
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
#include "umicom/studio/view_models.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiStudioViewModelSnapshot s;UmiStudioViewModels*m;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);m=umi_studio_ui_view_models(umi_studio_bootstrap_ui(b));assert(umi_studio_view_models_refresh(m)==UMI_STATUS_OK);assert(umi_studio_view_models_snapshot(m,&s)==UMI_STATUS_OK);assert(s.view_count==5U);assert(umi_studio_view_models_find(m,"studio.view.editor")!=NULL);umi_studio_bootstrap_destroy(b);return 0;}

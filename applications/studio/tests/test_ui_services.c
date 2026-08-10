/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui_services.c
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
int main(void){UmiStudioBootstrap*b=NULL;UmiServiceRegistry*r;assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);r=umi_studio_bootstrap_service_registry(b);assert(umi_service_registry_find(r,"umicom.studio.ui")!=NULL);assert(umi_service_registry_find(r,"umicom.ui.workbench")!=NULL);assert(umi_service_registry_find(r,"umicom.ui.application-shell")!=NULL);umi_studio_bootstrap_destroy(b);return 0;}

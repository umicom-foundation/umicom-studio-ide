/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ui_catalogue.c
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
#include "umicom/studio/ui_catalogue.h"
int main(void){UmiStudioBootstrap*b=NULL;UmiStudioUiCatalogue c;char text[2048];assert(umi_studio_bootstrap_create(&b)==UMI_STATUS_OK);assert(umi_studio_ui_catalogue_snapshot(umi_studio_bootstrap_ui(b),&c)==UMI_STATUS_OK);assert(c.perspectives==8U);assert(umi_studio_ui_catalogue_format(&c,text,sizeof(text))==UMI_STATUS_OK);assert(strstr(text,"Studio UI Catalogue")!=NULL);umi_studio_bootstrap_destroy(b);return 0;}

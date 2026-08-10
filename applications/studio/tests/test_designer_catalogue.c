/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_designer_catalogue.c
 *
 * PURPOSE:
 *   Verify Studio reports the semantic component palette exposed by Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/studio/declarative.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/designer_catalogue.h"
#include "umicom/studio/designer_preview.h"
#include "umicom/studio/new_project.h"
#include "umicom/studio/designer_state.h"
#include "umicom/studio/designer_project.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void){UmiStudioDeclarative *decl=NULL;char report[8192];assert(umi_studio_declarative_create(&decl)==UMI_STATUS_OK);assert(umi_studio_designer_catalogue_report(decl,"Development",report,sizeof(report))==UMI_STATUS_OK);assert(strstr(report,"editor")!=NULL);assert(strstr(report,"terminal")!=NULL);umi_studio_declarative_destroy(decl);return EXIT_SUCCESS;}

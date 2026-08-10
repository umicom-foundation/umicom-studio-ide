/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_new_project.c
 *
 * PURPOSE:
 *   Verify Studio generates declarative starter projects rather than toolkit-specific widget construction.
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

int main(void){char source[8192];UmiStudioDeclarative *decl=NULL;UmiDeclDocument *document=NULL;UmiDeclDiagnosticList diagnostics;assert(umi_studio_new_project_template(UMI_STUDIO_TEMPLATE_EDITOR,"org.umicom.example",source,sizeof(source))==UMI_STATUS_OK);assert(strstr(source,"component editor editor")!=NULL);assert(umi_studio_declarative_create(&decl)==UMI_STATUS_OK);assert(umi_studio_declarative_parse(decl,source,&document,&diagnostics)==UMI_STATUS_OK);umi_decl_document_destroy(document);umi_studio_declarative_destroy(decl);return EXIT_SUCCESS;}

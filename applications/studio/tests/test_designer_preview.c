/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_designer_preview.c
 *
 * PURPOSE:
 *   Verify Studio can generate both deterministic preview text and starter C source from one semantic document.
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

int main(void){UmiStudioDeclarative *decl=NULL;UmiStudioDesigner *designer=NULL;UmiDeclDiagnosticList diagnostics;char preview[2048],source[4096];assert(umi_studio_declarative_create(&decl)==UMI_STATUS_OK);assert(umi_studio_designer_create(decl,&designer)==UMI_STATUS_OK);assert(umi_studio_designer_generate_preview(designer,preview,sizeof(preview),&diagnostics)==UMI_STATUS_OK);assert(umi_studio_designer_generate_c(designer,decl,source,sizeof(source),&diagnostics)==UMI_STATUS_OK);assert(strstr(source,"int main")!=NULL);umi_studio_designer_destroy(designer);umi_studio_declarative_destroy(decl);return EXIT_SUCCESS;}

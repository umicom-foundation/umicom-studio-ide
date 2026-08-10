/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_declarative.c
 *
 * PURPOSE:
 *   Verify Studio parsing, validation and compilation over the Framework declarative subsystem.
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

int main(void){UmiStudioDeclarative *service=NULL;UmiDeclDocument *document=NULL;UmiDeclDiagnosticList diagnostics;UmiDeclApplicationPlan plan;const char *source="application org.umicom.test\ncomponent root window -\n";assert(umi_studio_declarative_create(&service)==UMI_STATUS_OK);assert(umi_studio_declarative_parse(service,source,&document,&diagnostics)==UMI_STATUS_OK);assert(umi_studio_declarative_compile(service,document,&plan,&diagnostics)==UMI_STATUS_OK);assert(plan.component_count==1U);umi_decl_plan_dispose(&plan);umi_decl_document_destroy(document);umi_studio_declarative_destroy(service);return EXIT_SUCCESS;}

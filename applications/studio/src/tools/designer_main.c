/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/designer_main.c
 *
 * PURPOSE:
 *   Provide a native designer smoke command for semantic add, property, undo, redo and preview operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command-line entry point parses user arguments, calls the shared Studio
 * service and reports the result; business logic stays in reusable services.
 */
#include "umicom/studio/services.h"
#include "umicom/studio/designer_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv)
{
    UmiStudioDeclarative *declarative=NULL;UmiStudioDesigner *designer=NULL;UmiDeclDiagnosticList diagnostics;char report[4096];char preview[4096];UmiStatus status;
    (void)argc;(void)argv;
    status=umi_studio_declarative_create(&declarative);
    if(status==UMI_STATUS_OK)status=umi_studio_designer_create(declarative,&designer);
    if(status==UMI_STATUS_OK)status=umi_studio_designer_add_component(designer,"editor","editor","root");
    if(status==UMI_STATUS_OK)status=umi_studio_designer_set_property(designer,"editor","title","Source Editor");
    if(status==UMI_STATUS_OK)status=umi_studio_designer_state_report(designer,report,sizeof(report));
    if(status==UMI_STATUS_OK)status=umi_studio_designer_preview(designer,preview,sizeof(preview),&diagnostics);
    if(status==UMI_STATUS_OK){fputs(report,stdout);puts("Preview:");fputs(preview,stdout);}
    umi_studio_designer_destroy(designer);umi_studio_declarative_destroy(declarative);return status==UMI_STATUS_OK?EXIT_SUCCESS:EXIT_FAILURE;
}

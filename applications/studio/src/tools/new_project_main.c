/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/new_project_main.c
 *
 * PURPOSE:
 *   Generate starter declarative source for a new Umicom desktop, editor or dashboard application.
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
#include "umicom/studio/new_project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv)
{
    UmiStudioProjectTemplate kind=UMI_STUDIO_TEMPLATE_DESKTOP;char source[8192];UmiStatus status;
    if(argc<2){fprintf(stderr,"Usage: umicom-studio-new-project <application-id> [desktop|editor|dashboard]\n");return EXIT_FAILURE;}
    if(argc>2&&strcmp(argv[2],"editor")==0)kind=UMI_STUDIO_TEMPLATE_EDITOR;else if(argc>2&&strcmp(argv[2],"dashboard")==0)kind=UMI_STUDIO_TEMPLATE_DASHBOARD;
    status=umi_studio_new_project_template(kind,argv[1],source,sizeof(source));if(status==UMI_STATUS_OK)fputs(source,stdout);return status==UMI_STATUS_OK?EXIT_SUCCESS:EXIT_FAILURE;
}

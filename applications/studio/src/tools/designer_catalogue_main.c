/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/designer_catalogue_main.c
 *
 * PURPOSE:
 *   List semantic components available to the visual designer, optionally filtered by category.
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
#include "umicom/studio/designer_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv)
{
    UmiStudioDeclarative *service=NULL;char report[16384];const char *filter=argc>1?argv[1]:NULL;UmiStatus status=umi_studio_declarative_create(&service);
    if(status==UMI_STATUS_OK)status=umi_studio_designer_catalogue_report(service,filter,report,sizeof(report));
    if (status == UMI_STATUS_OK) {
        fputs(report, stdout);
    }
    umi_studio_declarative_destroy(service);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

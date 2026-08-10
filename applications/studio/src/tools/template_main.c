/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/template_main.c
 *
 * PURPOSE:
 *   Provide a native command for validating and formatting .umiapp source without launching GTK4.
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
#include "umicom/studio/declarative.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_file(const char *path,char **out_text)
{
    FILE *file; long length; char *text;
    *out_text=NULL; file=fopen(path,"rb"); if(file==NULL)return 0;
    if(fseek(file,0L,SEEK_END)!=0){fclose(file);return 0;} length=ftell(file);
    if(length<0L||fseek(file,0L,SEEK_SET)!=0){fclose(file);return 0;}
    text=(char *)malloc((size_t)length+1U); if(text==NULL){fclose(file);return 0;}
    if(length>0L&&fread(text,1U,(size_t)length,file)!=(size_t)length){free(text);fclose(file);return 0;}
    text[(size_t)length]='\0'; fclose(file); *out_text=text; return 1;
}
int main(int argc,char **argv)
{
    UmiStudioDeclarative *service=NULL; UmiDeclDocument *document=NULL; UmiDeclDiagnosticList diagnostics; char *source=NULL; char formatted[65536]; UmiStatus status;
    if(argc<3){fprintf(stderr,"Usage: umicom-studio-template validate|format <file.umiapp>\n");return EXIT_FAILURE;}
    if(!read_file(argv[2],&source)){fprintf(stderr,"Unable to read %s\n",argv[2]);return EXIT_FAILURE;}
    status=umi_studio_declarative_create(&service);
    if(status==UMI_STATUS_OK&&strcmp(argv[1],"format")==0)status=umi_studio_declarative_format(service,source,formatted,sizeof(formatted),&diagnostics);
    else if(status==UMI_STATUS_OK){status=umi_studio_declarative_parse(service,source,&document,&diagnostics);if(status==UMI_STATUS_OK)status=umi_studio_declarative_validate(service,document,&diagnostics);}
    if(status==UMI_STATUS_OK){if(strcmp(argv[1],"format")==0)fputs(formatted,stdout);else printf("Template is valid. Diagnostics: %zu\n",diagnostics.count);}else{size_t i;for(i=0U;i<diagnostics.count;++i)fprintf(stderr,"line %zu: %s\n",diagnostics.items[i].line,diagnostics.items[i].message);}
    umi_decl_document_destroy(document);umi_studio_declarative_destroy(service);free(source);return status==UMI_STATUS_OK?EXIT_SUCCESS:EXIT_FAILURE;
}

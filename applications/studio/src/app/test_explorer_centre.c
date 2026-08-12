/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/test_explorer_centre.c
 *
 * PURPOSE:
 *   Implement the Test Explorer Centre composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/test_explorer_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioTestExplorerCentre { UmiTestPlatformService *service; uint64_t revision; };
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_test_explorer_centre_create(UmiStudioTestExplorerCentre **out){UmiStudioTestExplorerCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_test_platform_service_create(&p->service);if(s!=UMI_STATUS_OK){free(p);return s;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_test_explorer_centre_destroy(UmiStudioTestExplorerCentre*p){if(p==NULL)return;umi_test_platform_service_destroy(p->service);free(p);}
UmiStatus umi_studio_test_explorer_centre_snapshot(UmiStudioTestExplorerCentre*p,UmiStudioTestExplorerCentreSnapshot*o){UmiStatus s;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.test-explorer-centre");copy_text(o->title,sizeof(o->title),"Test Explorer Centre");copy_text(o->summary,sizeof(o->summary),"Test discovery, hierarchy, run profiles, sessions, results, output, coverage and benchmarks.");s=umi_test_platform_service_snapshot(p->service,&o->service);if(s!=UMI_STATUS_OK)return s;o->revision=p->revision;o->available=1;return UMI_STATUS_OK;}
UmiTestPlatformService *umi_studio_test_explorer_centre_service(UmiStudioTestExplorerCentre*p){return p!=NULL?p->service:NULL;}

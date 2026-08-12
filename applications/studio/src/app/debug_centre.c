/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/debug_centre.c
 *
 * PURPOSE:
 *   Implement the Debug Centre composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/debug_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioDebugCentre { UmiDebugService *service; uint64_t revision; };
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_debug_centre_create(UmiStudioDebugCentre **out){UmiStudioDebugCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_debug_service_create(&p->service);if(s!=UMI_STATUS_OK){free(p);return s;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_debug_centre_destroy(UmiStudioDebugCentre*p){if(p==NULL)return;umi_debug_service_destroy(p->service);free(p);}
UmiStatus umi_studio_debug_centre_snapshot(UmiStudioDebugCentre*p,UmiStudioDebugCentreSnapshot*o){UmiStatus s;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.debug-centre");copy_text(o->title,sizeof(o->title),"Debug Centre");copy_text(o->summary,sizeof(o->summary),"Launch configurations, sessions, breakpoints, stacks, variables, watches and debugger events.");s=umi_debug_service_snapshot(p->service,&o->service);if(s!=UMI_STATUS_OK)return s;o->revision=p->revision;o->available=1;return UMI_STATUS_OK;}
UmiDebugService *umi_studio_debug_centre_service(UmiStudioDebugCentre*p){return p!=NULL?p->service:NULL;}

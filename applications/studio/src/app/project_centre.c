/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/project_centre.c
 *
 * PURPOSE:
 *   Implement the Project Centre composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/project_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioProjectCentre { UmiProjectWorkspace *service; uint64_t revision; };
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_project_centre_create(UmiStudioProjectCentre **out){UmiStudioProjectCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_project_workspace_create(&p->service);if(s!=UMI_STATUS_OK){free(p);return s;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_project_centre_destroy(UmiStudioProjectCentre*p){if(p==NULL)return;umi_project_workspace_destroy(p->service);free(p);}
UmiStatus umi_studio_project_centre_snapshot(UmiStudioProjectCentre*p,UmiStudioProjectCentreSnapshot*o){UmiStatus s;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.project-centre");copy_text(o->title,sizeof(o->title),"Project Centre");copy_text(o->summary,sizeof(o->summary),"Projects, targets, configurations, dependencies, tasks, launch profiles and build graph.");s=umi_project_workspace_snapshot(p->service,&o->service);if(s!=UMI_STATUS_OK)return s;o->revision=p->revision;o->available=1;return UMI_STATUS_OK;}
UmiProjectWorkspace *umi_studio_project_centre_service(UmiStudioProjectCentre*p){return p!=NULL?p->service:NULL;}

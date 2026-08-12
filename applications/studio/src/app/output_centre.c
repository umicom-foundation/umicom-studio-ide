/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/output_centre.c
 *
 * PURPOSE:
 *   Implement the Output Centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/output_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioOutputCentre{UmiUiWorkbenchServices*services;uint64_t revision;};
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_output_centre_create(UmiUiWorkbenchServices*s,UmiStudioOutputCentre**out){UmiStudioOutputCentre*p;if(s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;p->services=s;p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_output_centre_destroy(UmiStudioOutputCentre*p){free(p);}
UmiUiOutputChannelRegistry *umi_studio_output_centre_registry(UmiStudioOutputCentre*p){return p!=NULL?umi_ui_workbench_services_output_channel(p->services):NULL;}
UmiStatus umi_studio_output_centre_snapshot(UmiStudioOutputCentre*p,UmiStudioOutputCentreSnapshot*o){UmiUiOutputChannelRegistry*r;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.output-centre");copy_text(o->title,sizeof(o->title),"Output Centre");copy_text(o->summary,sizeof(o->summary),"Named output channels for builds, tests, tools, terminals and application services.");r=umi_studio_output_centre_registry(p);if(r==NULL)return UMI_STATUS_INVALID_STATE;o->item_count=umi_ui_output_channel_registry_count(r);o->revision=p->revision;o->available=1;return UMI_STATUS_OK;}

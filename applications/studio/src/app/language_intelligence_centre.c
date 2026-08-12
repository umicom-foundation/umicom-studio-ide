/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/language_intelligence_centre.c
 *
 * PURPOSE:
 *   Implement the Language Intelligence Centre composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/language_intelligence_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioLanguageIntelligenceCentre { UmiLanguageService *service; uint64_t revision; };
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_language_intelligence_centre_create(UmiStudioLanguageIntelligenceCentre **out){UmiStudioLanguageIntelligenceCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_language_service_create(&p->service);if(s!=UMI_STATUS_OK){free(p);return s;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_language_intelligence_centre_destroy(UmiStudioLanguageIntelligenceCentre*p){if(p==NULL)return;umi_language_service_destroy(p->service);free(p);}
UmiStatus umi_studio_language_intelligence_centre_snapshot(UmiStudioLanguageIntelligenceCentre*p,UmiStudioLanguageIntelligenceCentreSnapshot*o){UmiStatus s;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.language-intelligence-centre");copy_text(o->title,sizeof(o->title),"Language Intelligence Centre");copy_text(o->summary,sizeof(o->summary),"Provider-neutral completion, symbols, navigation, diagnostics, refactoring and semantic information.");s=umi_language_service_snapshot(p->service,&o->service);if(s!=UMI_STATUS_OK)return s;o->revision=p->revision;o->available=1;return UMI_STATUS_OK;}
UmiLanguageService *umi_studio_language_intelligence_centre_service(UmiStudioLanguageIntelligenceCentre*p){return p!=NULL?p->service:NULL;}

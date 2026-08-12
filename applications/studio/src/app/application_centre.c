/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/application_centre.c
 *
 * PURPOSE:
 *   Implement Studio application-centre composition for Umicom products and runtimes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include "umicom/studio/application_centre.h"
#include <string.h>
static void copy_text(char *dst,size_t cap,const char *src){size_t len;if(dst==NULL||cap==0U)return;if(src==NULL)src="";len=strlen(src);if(len>=cap)len=cap-1U;if(len>0U)memcpy(dst,src,len);dst[len]='\0';}
UmiStatus umi_studio_application_centre_snapshot(UmiStudioServices *services,UmiStudioApplicationCentreSnapshot *out)
{
    (void)services;
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out)); out->struct_size=(uint32_t)sizeof(*out); out->api_version=1U;
    copy_text(out->area_id,sizeof(out->area_id),"studio.application-centre");
    copy_text(out->view_type,sizeof(out->view_type),"studio.application-centre");
    copy_text(out->title,sizeof(out->title),"ApplicationCentre");
    copy_text(out->summary,sizeof(out->summary),"Discover, launch and inspect independently deployable Umicom applications.");
    out->revision=1U; out->available=1;
    return UMI_STATUS_OK;
}

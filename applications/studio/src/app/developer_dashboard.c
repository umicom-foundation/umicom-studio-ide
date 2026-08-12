/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_dashboard.c
 *
 * PURPOSE:
 *   Implement a combined Studio build, test, debug, terminal and source-control dashboard.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include "umicom/studio/developer_dashboard.h"
#include <string.h>
static void copy_text(char *dst,size_t cap,const char *src){size_t len;if(dst==NULL||cap==0U)return;if(src==NULL)src="";len=strlen(src);if(len>=cap)len=cap-1U;if(len>0U)memcpy(dst,src,len);dst[len]='\0';}
UmiStatus umi_studio_developer_dashboard_snapshot(UmiStudioServices *services,UmiStudioDeveloperDashboardSnapshot *out)
{
    (void)services;
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out)); out->struct_size=(uint32_t)sizeof(*out); out->api_version=1U;
    copy_text(out->area_id,sizeof(out->area_id),"studio.developer-dashboard");
    copy_text(out->view_type,sizeof(out->view_type),"studio.developer-dashboard");
    copy_text(out->title,sizeof(out->title),"DeveloperDashboard");
    copy_text(out->summary,sizeof(out->summary),"Build, tests, debugger, terminal, source control and diagnostics status.");
    out->revision=1U; out->available=1;
    return UMI_STATUS_OK;
}

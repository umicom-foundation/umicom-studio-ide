/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ai_workspace.c
 *
 * PURPOSE:
 *   Implement Studio AI workspace composing AuthorEngine, retrieval, tools and Helix governance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include "umicom/studio/ai_workspace.h"
#include <string.h>
static void copy_text(char *dst,size_t cap,const char *src){size_t len;if(dst==NULL||cap==0U)return;if(src==NULL)src="";len=strlen(src);if(len>=cap)len=cap-1U;if(len>0U)memcpy(dst,src,len);dst[len]='\0';}
UmiStatus umi_studio_ai_workspace_snapshot(UmiStudioServices *services,UmiStudioAiWorkspaceSnapshot *out)
{
    UmiStudioAiPlatform *platform;
    UmiAiAuthorEngineServiceSnapshot integration;
    UmiStatus status;
    if(services==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out)); out->struct_size=(uint32_t)sizeof(*out); out->api_version=1U;
    copy_text(out->area_id,sizeof(out->area_id),"studio.ai-workspace");
    copy_text(out->view_type,sizeof(out->view_type),"studio.ai-workspace");
    copy_text(out->title,sizeof(out->title),"AI Workspace");
    copy_text(out->summary,sizeof(out->summary),"AuthorEngine-orchestrated providers, governed project context, conversation sessions and privacy controls.");
    platform=umi_studio_services_ai_platform(services);
    if(platform==NULL)return UMI_STATUS_INVALID_STATE;
    status=umi_studio_ai_platform_snapshot(platform,&integration);
    if(status!=UMI_STATUS_OK)return status;
    out->revision=integration.revision;
    out->provider_count=integration.providers;
    out->runtime_count=integration.runtimes;
    out->healthy_runtime_count=integration.healthy_runtimes;
    out->context_source_count=integration.context_sources;
    out->session_count=integration.sessions;
    out->item_count=integration.runtimes+integration.context_sources+
                    integration.sessions;
    out->context_limit=integration.context_limit;
    out->reserved_output_tokens=integration.reserved_output_tokens;
    out->remote_allowed=integration.remote_allowed;
    out->persistence_allowed=integration.persistence_allowed;
    copy_text(out->active_session_id,sizeof(out->active_session_id),
              integration.active_session_id);
    out->available=1;
    return status;
}

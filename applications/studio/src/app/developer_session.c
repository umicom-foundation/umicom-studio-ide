/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_session.c
 * PURPOSE: Implement the active developer context with bounded value semantics.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_session.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioDeveloperSession{UmiStudioDeveloperSessionSnapshot state;};
static void normalise(UmiStudioDeveloperSessionSnapshot*s){s->struct_size=(uint32_t)sizeof(*s);s->api_version=1U;s->project_id[127U]='\0';s->configuration_id[127U]='\0';s->launch_profile_id[127U]='\0';s->repository_id[127U]='\0';s->test_item_id[127U]='\0';s->active_file[511U]='\0';}
UmiStatus umi_studio_developer_session_create(UmiStudioDeveloperSession**out){UmiStudioDeveloperSession*p;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;p->state.revision=1U;normalise(&p->state);*out=p;return UMI_STATUS_OK;}
void umi_studio_developer_session_destroy(UmiStudioDeveloperSession*p){free(p);}
UmiStatus umi_studio_developer_session_set_context(UmiStudioDeveloperSession*p,const UmiStudioDeveloperSessionSnapshot*c){if(p==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;p->state=*c;p->state.revision+=1U;normalise(&p->state);return UMI_STATUS_OK;}
UmiStatus umi_studio_developer_session_snapshot(const UmiStudioDeveloperSession*p,UmiStudioDeveloperSessionSnapshot*o){if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;*o=p->state;return UMI_STATUS_OK;}

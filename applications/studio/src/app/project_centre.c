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
struct UmiStudioProjectCentre { UmiProjectWorkspace *service; UmiProjectWorkspaceSelectionSnapshot selection; UmiProjectWorkspaceValidationReport validation; int has_selection; uint64_t revision; };
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_project_centre_create(UmiStudioProjectCentre **out){UmiStudioProjectCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_project_workspace_create(&p->service);if(s!=UMI_STATUS_OK){free(p);return s;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_project_centre_destroy(UmiStudioProjectCentre*p){if(p==NULL)return;umi_project_workspace_destroy(p->service);free(p);}
UmiStatus umi_studio_project_centre_snapshot(UmiStudioProjectCentre*p,UmiStudioProjectCentreSnapshot*o){UmiStatus s;if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.project-centre");copy_text(o->title,sizeof(o->title),"Project Centre");copy_text(o->summary,sizeof(o->summary),"Projects, targets, configurations, dependencies, tasks, launch profiles and build graph.");s=umi_project_workspace_snapshot(p->service,&o->service);if(s!=UMI_STATUS_OK)return s;s=umi_project_workspace_validate(p->service,&p->validation);if(s!=UMI_STATUS_OK)return s;o->validation=p->validation;if(p->has_selection){o->selection=p->selection;o->has_selection=1;}o->revision=p->revision+o->service.revision;o->available=1;return UMI_STATUS_OK;}
UmiProjectWorkspace *umi_studio_project_centre_service(UmiStudioProjectCentre*p){return p!=NULL?p->service:NULL;}

UmiStatus umi_studio_project_centre_select(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection)
{
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiStatus status;

    if (centre == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_project_workspace_resolve_selection(
        centre->service, request, &selection);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    centre->selection = selection;
    centre->has_selection = 1;
    centre->revision += 1U;
    if (out_selection != NULL) {
        *out_selection = selection;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_project_centre_select_project(
    UmiStudioProjectCentre *centre,
    const char *project_id,
    UmiProjectWorkspaceSelectionSnapshot *out_selection)
{
    UmiProjectWorkspaceSelectionRequest request;

    if (centre == NULL || project_id == NULL || project_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    request.project_id = project_id;
    return umi_studio_project_centre_select(centre, &request, out_selection);
}

UmiStatus umi_studio_project_centre_current_selection(
    UmiStudioProjectCentre *centre,
    UmiProjectWorkspaceSelectionSnapshot *out_selection)
{
    if (centre == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!centre->has_selection) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_selection = centre->selection;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_project_centre_validate(
    UmiStudioProjectCentre *centre,
    UmiProjectWorkspaceValidationReport *out_report)
{
    UmiStatus status;

    if (centre == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_project_workspace_validate(centre->service, &centre->validation);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    *out_report = centre->validation;
    return UMI_STATUS_OK;
}

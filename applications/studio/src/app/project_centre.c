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
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>

struct UmiStudioProjectCentre {
    UmiProjectWorkspace *service;
    UmiProjectWorkspaceModel *workspace_model;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    UmiProjectWorkspaceRefreshSnapshot refresh_plan;
    int has_selection;
    int has_refresh_plan;
    uint64_t revision;
};
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}

UmiStatus umi_studio_project_centre_create(UmiStudioProjectCentre **out)
{
    UmiStudioProjectCentre *centre;
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    centre = (UmiStudioProjectCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_project_workspace_create(&centre->service);
    if (status == UMI_STATUS_OK)
        status = umi_project_workspace_model_create(
            centre->service, &centre->workspace_model);
    if (status != UMI_STATUS_OK) {
        umi_project_workspace_destroy(centre->service);
        free(centre);
        return status;
    }
    centre->revision = 1U;
    *out = centre;
    return UMI_STATUS_OK;
}

void umi_studio_project_centre_destroy(UmiStudioProjectCentre *centre)
{
    if (centre == NULL) return;
    umi_project_workspace_model_destroy(centre->workspace_model);
    umi_project_workspace_destroy(centre->service);
    free(centre);
}

UmiStatus umi_studio_project_centre_snapshot(
    UmiStudioProjectCentre *centre,
    UmiStudioProjectCentreSnapshot *out_snapshot)
{
    UmiStatus status;
    if (centre == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_STUDIO_PROJECT_CENTRE_API_VERSION;
    copy_text(out_snapshot->area_id, sizeof(out_snapshot->area_id),
              "studio.project-centre");
    copy_text(out_snapshot->title, sizeof(out_snapshot->title),
              "Project Centre");
    copy_text(out_snapshot->summary, sizeof(out_snapshot->summary),
              "Multi-root projects, inherited settings, CMake discovery, "
              "tasks, launch profiles and deterministic build order.");
    status = umi_project_workspace_snapshot(
        centre->service, &out_snapshot->service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_workspace_model_snapshot(
        centre->workspace_model, &out_snapshot->workspace_model);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->has_workspace_model = 1;
    out_snapshot->build_order_status =
        umi_project_workspace_model_resolve_build_order(
            centre->workspace_model, &out_snapshot->build_order);
    out_snapshot->has_build_order =
        out_snapshot->build_order_status == UMI_STATUS_OK;
    status = umi_project_workspace_validate(
        centre->service, &centre->validation);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->validation = centre->validation;
    if (centre->has_selection) {
        out_snapshot->selection = centre->selection;
        out_snapshot->has_selection = 1;
    }
    if (centre->has_refresh_plan) {
        out_snapshot->refresh.source_revision =
            centre->refresh_plan.source_revision;
        out_snapshot->refresh.discovered_project_count =
            centre->refresh_plan.discovery.project_count;
        out_snapshot->refresh.existing_project_count =
            centre->refresh_plan.existing_project_count;
        out_snapshot->refresh.unchanged_project_count =
            centre->refresh_plan.unchanged_project_count;
        out_snapshot->refresh.import_candidate_count =
            centre->refresh_plan.import_candidate_count;
        out_snapshot->refresh.missing_project_count =
            centre->refresh_plan.missing_project_count;
        out_snapshot->refresh.requires_confirmation =
            centre->refresh_plan.requires_confirmation;
        out_snapshot->has_refresh_plan = 1;
    }
    out_snapshot->revision = centre->revision +
                             out_snapshot->workspace_model.revision;
    out_snapshot->available = 1;
    return UMI_STATUS_OK;
}
UmiProjectWorkspace *umi_studio_project_centre_service(UmiStudioProjectCentre*p){return p!=NULL?p->service:NULL;}

UmiProjectWorkspaceModel *umi_studio_project_centre_workspace_model(
    UmiStudioProjectCentre *centre)
{
    return centre != NULL ? centre->workspace_model : NULL;
}

UmiStatus umi_studio_project_centre_upsert_workspace_root(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceRootSnapshot *root)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_upsert_root(
        centre->workspace_model, root);
    if (status == UMI_STATUS_OK) {
        centre->has_refresh_plan = 0;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_project_centre_upsert_project_group(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceGroupSnapshot *group)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_upsert_group(
        centre->workspace_model, group);
    if (status == UMI_STATUS_OK) {
        centre->has_refresh_plan = 0;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_project_centre_upsert_project_member(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceMemberSnapshot *member)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_upsert_member(
        centre->workspace_model, member);
    if (status == UMI_STATUS_OK) {
        centre->has_refresh_plan = 0;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_project_centre_upsert_workspace_setting(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceSettingSnapshot *setting)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_upsert_setting(
        centre->workspace_model, setting);
    if (status == UMI_STATUS_OK) {
        centre->has_refresh_plan = 0;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_project_centre_upsert_workspace_exclusion(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceExclusionSnapshot *exclusion)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_upsert_exclusion(
        centre->workspace_model, exclusion);
    if (status == UMI_STATUS_OK) {
        centre->has_refresh_plan = 0;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_project_centre_plan_workspace_refresh(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceRefreshSnapshot *out_refresh)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_project_workspace_model_plan_refresh(
        centre->workspace_model, options, &centre->refresh_plan);
    if (status != UMI_STATUS_OK) return status;
    centre->has_refresh_plan = 1;
    centre->revision += 1U;
    if (out_refresh != NULL) *out_refresh = centre->refresh_plan;
    return UMI_STATUS_OK;
}

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
    if (request != NULL && request->project_id != NULL) {
        UmiProjectWorkspaceMemberSnapshot member;
        if (umi_project_workspace_model_find_project_member(
                centre->workspace_model, request->project_id, &member) ==
            UMI_STATUS_OK)
            (void)umi_project_workspace_model_set_active_project(
                centre->workspace_model, request->project_id);
    }
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

UmiStatus umi_studio_project_centre_import_directory(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceImportRequest *request,
    UmiProjectWorkspaceImportSnapshot *out_snapshot)
{
    UmiProjectWorkspaceRootSnapshot root;
    UmiProjectWorkspaceMemberSnapshot member;
    UmiStatus status;
    size_t root_index;
    int reused_root = 0;

    if (centre == NULL || request == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

#define imported (*out_snapshot)
    status = umi_project_workspace_import_directory(
        centre->service, request, &imported);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /*
     * Import remains non-destructive.  The imported project directory becomes
     * a model root only when no root with the same stable identifier exists;
     * upsert then makes repeated imports idempotent.  Root/member identifiers
     * live in separate registries, so the project identifier is a safe and
     * deterministic identity for both records without string-prefix overflow.
     */
    memset(&root, 0, sizeof(root));
    for (root_index = 0U;
         root_index < umi_project_workspace_model_root_count(
             centre->workspace_model);
         ++root_index) {
        if (umi_project_workspace_model_root_at(
                centre->workspace_model, root_index, &root) == UMI_STATUS_OK &&
            umi_path_equal(root.path, imported.root_directory)) {
            reused_root = 1;
            break;
        }
    }
    if (!reused_root) {
        memset(&root, 0, sizeof(root));
        root.struct_size = (uint32_t)sizeof(root);
        root.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
        copy_text(root.id, sizeof(root.id), imported.project_id);
        copy_text(root.path, sizeof(root.path), imported.root_directory);
        copy_text(root.label, sizeof(root.label), imported.display_name);
        root.order = (int32_t)umi_project_workspace_model_root_count(
            centre->workspace_model);
        root.enabled = 1;
        status = umi_project_workspace_model_upsert_root(
            centre->workspace_model, &root);
        if (status != UMI_STATUS_OK) return status;
    }
    memset(&member, 0, sizeof(member));
    member.struct_size = (uint32_t)sizeof(member);
    member.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    copy_text(member.id, sizeof(member.id), imported.project_id);
    copy_text(member.root_id, sizeof(member.root_id), imported.project_id);
    copy_text(member.project_id, sizeof(member.project_id), imported.project_id);
    member.order = (int32_t)umi_project_workspace_model_member_count(
        centre->workspace_model);
    member.enabled = 1;
    status = umi_project_workspace_model_upsert_member(
        centre->workspace_model, &member);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_workspace_model_set_active_project(
        centre->workspace_model, imported.project_id);
    if (status != UMI_STATUS_OK) return status;

    centre->selection = imported.selection;
    centre->validation = imported.validation;
    centre->has_selection = 1;
    centre->has_refresh_plan = 0;
    centre->revision += 1U;
#undef imported
    return UMI_STATUS_OK;
}

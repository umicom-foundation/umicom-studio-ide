/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/project_centre.h
 *
 * PURPOSE:
 *   Compose the Project Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_PROJECT_CENTRE_H
#define UMICOM_STUDIO_PROJECT_CENTRE_H
#include <stdint.h>
#include "umicom/project/project.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioProjectCentre UmiStudioProjectCentre;
#define UMI_STUDIO_PROJECT_CENTRE_API_VERSION 2U
typedef struct UmiStudioProjectCentreRefreshSummary {
    uint64_t source_revision;
    size_t discovered_project_count;
    size_t existing_project_count;
    size_t unchanged_project_count;
    size_t import_candidate_count;
    size_t missing_project_count;
    int requires_confirmation;
} UmiStudioProjectCentreRefreshSummary;
typedef struct UmiStudioProjectCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiProjectWorkspaceSnapshot service; uint64_t revision; int available;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    int has_selection;
    UmiProjectWorkspaceModelSnapshot workspace_model;
    UmiProjectWorkspaceOrderSnapshot build_order;
    UmiStudioProjectCentreRefreshSummary refresh;
    UmiStatus build_order_status;
    int has_workspace_model;
    int has_build_order;
    int has_refresh_plan;
} UmiStudioProjectCentreSnapshot;
UmiStatus umi_studio_project_centre_create(UmiStudioProjectCentre **out_centre);
void umi_studio_project_centre_destroy(UmiStudioProjectCentre *centre);
UmiStatus umi_studio_project_centre_snapshot(UmiStudioProjectCentre *centre,UmiStudioProjectCentreSnapshot *out_snapshot);
UmiProjectWorkspace *umi_studio_project_centre_service(UmiStudioProjectCentre *centre);
UmiProjectWorkspaceModel *umi_studio_project_centre_workspace_model(
    UmiStudioProjectCentre *centre);

UmiStatus umi_studio_project_centre_upsert_workspace_root(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceRootSnapshot *root);
UmiStatus umi_studio_project_centre_upsert_project_group(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceGroupSnapshot *group);
UmiStatus umi_studio_project_centre_upsert_project_member(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceMemberSnapshot *member);
UmiStatus umi_studio_project_centre_upsert_workspace_setting(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceSettingSnapshot *setting);
UmiStatus umi_studio_project_centre_upsert_workspace_exclusion(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceExclusionSnapshot *exclusion);
UmiStatus umi_studio_project_centre_plan_workspace_refresh(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceRefreshSnapshot *out_refresh);

UmiStatus umi_studio_project_centre_select(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_studio_project_centre_select_project(
    UmiStudioProjectCentre *centre,
    const char *project_id,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_studio_project_centre_current_selection(
    UmiStudioProjectCentre *centre,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_studio_project_centre_validate(
    UmiStudioProjectCentre *centre,
    UmiProjectWorkspaceValidationReport *out_report);

UmiStatus umi_studio_project_centre_import_directory(
    UmiStudioProjectCentre *centre,
    const UmiProjectWorkspaceImportRequest *request,
    UmiProjectWorkspaceImportSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif

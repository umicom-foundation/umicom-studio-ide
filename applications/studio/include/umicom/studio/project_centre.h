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
#include "umicom/project/workspace.h"
#include "umicom/project/workspace_query.h"
#include "umicom/project/workspace_validation.h"
#include "umicom/project/workspace_import.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioProjectCentre UmiStudioProjectCentre;
typedef struct UmiStudioProjectCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiProjectWorkspaceSnapshot service; uint64_t revision; int available;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    int has_selection;
} UmiStudioProjectCentreSnapshot;
UmiStatus umi_studio_project_centre_create(UmiStudioProjectCentre **out_centre);
void umi_studio_project_centre_destroy(UmiStudioProjectCentre *centre);
UmiStatus umi_studio_project_centre_snapshot(UmiStudioProjectCentre *centre,UmiStudioProjectCentreSnapshot *out_snapshot);
UmiProjectWorkspace *umi_studio_project_centre_service(UmiStudioProjectCentre *centre);

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

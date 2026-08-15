/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/test_explorer_centre.h
 *
 * PURPOSE:
 *   Compose the Test Explorer Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_TEST_EXPLORER_CENTRE_H
#define UMICOM_STUDIO_TEST_EXPLORER_CENTRE_H
#include <stdint.h>
#include "umicom/test_platform/service.h"
#include "umicom/studio/tests.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioTestExplorerCentre UmiStudioTestExplorerCentre;
typedef struct UmiStudioTestExplorerCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiTestPlatformServiceSnapshot service;
    UmiTestWorkspaceSnapshot workspace;
    UmiStudioTestExplorerState explorer;
    size_t hierarchy_count;
    size_t selected_count;
    uint64_t revision;
    int operation_running;
    int available;
} UmiStudioTestExplorerCentreSnapshot;
UmiStatus umi_studio_test_explorer_centre_create(UmiStudioTestExplorerCentre **out_centre);
UmiStatus umi_studio_test_explorer_centre_create_bound(
    UmiStudioTestService *test_service,
    UmiStudioTestExplorerCentre **out_centre
);
void umi_studio_test_explorer_centre_destroy(UmiStudioTestExplorerCentre *centre);
UmiStatus umi_studio_test_explorer_centre_snapshot(UmiStudioTestExplorerCentre *centre,UmiStudioTestExplorerCentreSnapshot *out_snapshot);
UmiTestPlatformService *umi_studio_test_explorer_centre_service(UmiStudioTestExplorerCentre *centre);
UmiTestWorkspace *umi_studio_test_explorer_centre_workspace(
    UmiStudioTestExplorerCentre *centre
);
UmiStatus umi_studio_test_explorer_centre_set_workspace(
    UmiStudioTestExplorerCentre *centre,
    const char *workspace_root,
    const char *project_id,
    uint64_t workspace_revision
);
UmiStatus umi_studio_test_explorer_centre_set_filter(
    UmiStudioTestExplorerCentre *centre,
    const char *search_text,
    const char *label,
    int outcome,
    int include_disabled
);
UmiStatus umi_studio_test_explorer_centre_hierarchy(
    UmiStudioTestExplorerCentre *centre,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count
);
UmiStatus umi_studio_test_explorer_centre_plan_all(
    UmiStudioTestExplorerCentre *centre,
    uint32_t repeat_count,
    int stop_on_failure,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_studio_test_explorer_centre_plan_failed(
    UmiStudioTestExplorerCentre *centre,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_studio_test_explorer_centre_begin(
    UmiStudioTestExplorerCentre *centre,
    const UmiTestPlatformOperationPlan *plan
);
UmiStatus umi_studio_test_explorer_centre_stop(
    UmiStudioTestExplorerCentre *centre
);
void umi_studio_test_explorer_centre_finish(UmiStudioTestExplorerCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

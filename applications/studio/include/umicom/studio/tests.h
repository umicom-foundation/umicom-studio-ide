/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/tests.h
 *
 * PURPOSE:
 *   Expose test discovery, execution and summaries through the Framework testing model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_TESTS_H
#define UMICOM_STUDIO_TESTS_H

#include <stddef.h>

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioTestService UmiStudioTestService;

typedef struct UmiStudioTestSnapshot {
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    size_t suite_count;
    size_t test_count;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t cancelled;
    size_t timed_out;
    size_t selected_count;
    size_t retained_result_count;
    size_t retained_attachment_count;
    uint64_t workspace_revision;
    uint64_t explorer_revision;
    int operation_running;
    int stop_requested;
} UmiStudioTestSnapshot;

typedef struct UmiStudioTestExplorerState {
    char workspace_root[UMI_BUILD_PATH_CAPACITY];
    char active_project_id[128];
    char active_suite_id[128];
    char search_text[256];
    char label_filter[128];
    int outcome_filter;
    int include_disabled;
    size_t selected_count;
    uint64_t workspace_revision;
    uint64_t revision;
} UmiStudioTestExplorerState;

UmiStatus umi_studio_test_service_create(UmiStudioTestService **out_service);
void umi_studio_test_service_destroy(UmiStudioTestService *service);
UmiStatus umi_studio_test_service_discover(UmiStudioTestService *service,
                                           const char *build_directory,
                                           size_t *out_discovered);
UmiStatus umi_studio_test_service_discover_metadata(
    UmiStudioTestService *service,
    const char *workspace_root,
    const char *project_id,
    const char *build_directory,
    const char *configuration,
    UmiTestPlatformCtestImportSummary *out_summary
);
/* Planning selects stable ids without starting child processes. Frontends can
 * inspect the plan before explicitly beginning or executing the operation. */
UmiStatus umi_studio_test_service_run_all(UmiStudioTestService *service,
                                          UmiCancellationToken *cancellation,
                                          UmiTestRunSummary *out_summary);
UmiStatus umi_studio_test_service_plan_all(
    UmiStudioTestService *service,
    uint32_t repeat_count,
    int stop_on_failure,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_studio_test_service_plan_selected(
    UmiStudioTestService *service,
    const char *const *item_ids,
    size_t item_count,
    uint32_t repeat_count,
    int stop_on_failure,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_studio_test_service_plan_failed(
    UmiStudioTestService *service,
    UmiTestPlatformOperationPlan *out_plan
);
UmiStatus umi_studio_test_service_begin(
    UmiStudioTestService *service,
    const UmiTestPlatformOperationPlan *plan
);
UmiStatus umi_studio_test_service_execute(
    UmiStudioTestService *service,
    const UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformExecutionSummary *out_summary
);
UmiStatus umi_studio_test_service_stop(UmiStudioTestService *service);
void umi_studio_test_service_finish(UmiStudioTestService *service);
UmiStatus umi_studio_test_service_set_filter(
    UmiStudioTestService *service,
    const char *search_text,
    const char *label,
    int outcome,
    int include_disabled
);
UmiStatus umi_studio_test_service_set_workspace(
    UmiStudioTestService *service,
    const char *workspace_root,
    const char *project_id,
    uint64_t workspace_revision
);
UmiStatus umi_studio_test_service_explorer_state(
    const UmiStudioTestService *service,
    UmiStudioTestExplorerState *out_state
);
UmiStatus umi_studio_test_service_hierarchy(
    UmiStudioTestService *service,
    UmiTestPlatformHierarchyNode *nodes,
    size_t capacity,
    size_t *out_count
);
UmiStatus umi_studio_test_service_snapshot(
    const UmiStudioTestService *service,
    UmiStudioTestSnapshot *out_snapshot
);
UmiTestRegistry *umi_studio_test_service_registry(
    UmiStudioTestService *service
);
UmiTestPlatformService *umi_studio_test_service_platform(
    UmiStudioTestService *service
);
/* Framework owns professional Test Explorer selection and presentation state;
 * Studio only exposes the coordinator bound to its application service. */
UmiTestWorkspace *umi_studio_test_service_workspace(
    UmiStudioTestService *service
);

#ifdef __cplusplus
}
#endif

#endif

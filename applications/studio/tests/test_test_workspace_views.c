/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_test_workspace_views.c
 *
 * PURPOSE:
 *   Verify Studio composes every professional testing pane from the single
 *   Framework-owned test workspace exposed by its test service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/contributions.h"
#include "umicom/studio/tests.h"
#include "umicom/ui/command_view.h"

static void add_evidence(UmiStudioTestService *tests)
{
    UmiTestPlatformService *service = umi_studio_test_service_platform(tests);
    UmiTestPlatformItemSnapshot item = {0};
    UmiTestPlatformResultSnapshot result = {0};
    UmiTestPlatformOutputSnapshot output = {0};
    UmiTestPlatformCoverageSnapshot coverage = {0};
    UmiTestPlatformRunSessionSnapshot session = {0};

    (void)strcpy(item.id, "studio.workspace.alpha");
    (void)strcpy(item.name, "Studio Workspace Alpha");
    (void)strcpy(item.kind, "test");
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(
               umi_test_platform_service_item(service), &item) ==
           UMI_STATUS_OK);
    (void)strcpy(result.id, "result.workspace.alpha");
    (void)strcpy(result.item_id, item.id);
    (void)strcpy(result.message, "Test failed");
    (void)strcpy(result.failure_details, "failure evidence");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.sequence = 1U;
    assert(umi_test_platform_result_registry_upsert(
               umi_test_platform_service_result(service), &result) ==
           UMI_STATUS_OK);
    (void)strcpy(output.id, "output.workspace.alpha");
    (void)strcpy(output.item_id, item.id);
    (void)strcpy(output.stream, "combined");
    (void)strcpy(output.text, "test output");
    assert(umi_test_platform_output_registry_upsert(
               umi_test_platform_service_output(service), &output) ==
           UMI_STATUS_OK);
    (void)strcpy(coverage.id, "coverage.workspace.alpha");
    (void)strcpy(coverage.uri, "applications/studio/src/app/tests.c");
    coverage.lines_total = 100U;
    coverage.lines_covered = 90U;
    assert(umi_test_platform_coverage_registry_upsert(
               umi_test_platform_service_coverage(service), &coverage) ==
           UMI_STATUS_OK);
    (void)strcpy(session.id, "run.workspace.alpha");
    session.total = 1U;
    session.failed = 1U;
    assert(umi_test_platform_run_session_registry_upsert(
               umi_test_platform_service_run_session(service), &session) ==
           UMI_STATUS_OK);
}

static void verify_view(UmiUiWorkbench *workbench, const char *view_type,
                        const char *pane_id, const char *expected_kind)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue kind;

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench), view_type, pane_id,
               &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &kind) == UMI_STATUS_OK);
    assert(kind.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(kind.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiUiWorkbench *workbench;
    UmiUiViewModel *explorer = NULL;
    UmiUiCommandViewAction action;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    add_evidence(umi_studio_services_tests(services));
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));

    verify_view(workbench, "studio.testing", UMI_STUDIO_PANE_TESTING,
                "test-explorer");
    verify_view(workbench, "studio.test-results", UMI_STUDIO_PANE_TEST_RESULTS,
                "test-results");
    verify_view(workbench, "studio.test-failures",
                UMI_STUDIO_PANE_TEST_FAILURES, "test-failures");
    verify_view(workbench, "studio.test-output", UMI_STUDIO_PANE_TEST_OUTPUT,
                "test-output");
    verify_view(workbench, "studio.test-coverage",
                UMI_STUDIO_PANE_TEST_COVERAGE, "test-coverage");
    verify_view(workbench, "studio.test-runs", UMI_STUDIO_PANE_TEST_RUNS,
                "test-runs");

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench), "studio.testing",
               UMI_STUDIO_PANE_TESTING, &explorer) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(explorer, 3U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.test.run-all") == 0);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(explorer, 7U, &action) ==
           UMI_STATUS_OK);
    assert(action.enabled);
    umi_ui_view_model_destroy(explorer);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

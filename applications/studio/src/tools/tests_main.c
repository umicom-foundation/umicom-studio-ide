/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/tests_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for Studio test discovery, execution and summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/services.h"
#include "umicom/studio/tests.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioTestService *service;
    UmiStudioTestSnapshot snapshot;
    UmiStatus status;
    size_t discovered = 0U;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Studio test service startup failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    service = umi_studio_services_tests(
        umi_studio_bootstrap_services(bootstrap));
    if (argc == 3 && strcmp(argv[1], "--discover") == 0) {
        status = umi_studio_test_service_discover(service, argv[2], &discovered);
        (void)printf("Discovered: %zu\nStatus: %s\n",
                     discovered, umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc >= 3 && strcmp(argv[1], "--discover-metadata") == 0) {
        UmiTestPlatformCtestImportSummary summary;
        const char *configuration = argc >= 4 ? argv[3] : "Debug";
        status = umi_studio_test_service_discover_metadata(
            service, ".", "studio", argv[2], configuration, &summary);
        (void)printf("Discovered: %zu\nDisabled: %zu\nLabelled: %zu\n"
                     "Timed: %zu\nStatus: %s\n",
                     summary.discovered_count, summary.disabled_count,
                     summary.labelled_count, summary.timed_count,
                     umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc == 2 && strcmp(argv[1], "--run") == 0) {
        UmiTestRunSummary summary;
        status = umi_studio_test_service_run_all(service, NULL, &summary);
        (void)printf("Passed: %zu\nFailed: %zu\nSkipped: %zu\n",
                     summary.passed, summary.failed, summary.skipped);
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc >= 3 && strcmp(argv[1], "--run-all") == 0) {
        UmiTestPlatformCtestImportSummary discovery_summary;
        UmiTestPlatformOperationPlan plan;
        UmiTestPlatformExecutionSummary execution;
        const char *configuration = argc >= 4 ? argv[3] : "Debug";
        status = umi_studio_test_service_discover_metadata(
            service, ".", "studio", argv[2], configuration,
            &discovery_summary);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_test_service_plan_all(service, 1U, 0, &plan);
        }
        if (status == UMI_STATUS_OK) {
            status = umi_studio_test_service_execute(service, &plan,
                                                     &execution);
        } else {
            (void)memset(&execution, 0, sizeof(execution));
        }
        (void)printf("Planned: %zu\nExecuted: %zu\nPassed: %zu\n"
                     "Failed: %zu\nSkipped: %zu\nStopped: %s\nStatus: %s\n",
                     execution.planned, execution.executed, execution.passed,
                     execution.failed, execution.skipped,
                     execution.stopped ? "yes" : "no",
                     umi_status_text(status));
        exit_code = status == UMI_STATUS_OK && execution.failed == 0U ? 0 : 1;
    } else if (argc >= 4 && strcmp(argv[1], "--repeat-all") == 0) {
        UmiTestPlatformCtestImportSummary discovery_summary;
        UmiTestPlatformOperationPlan plan;
        UmiTestPlatformExecutionSummary execution;
        unsigned parsed = 0U;
        const char *configuration = argc >= 5 ? argv[4] : "Debug";
        if (sscanf(argv[3], "%u", &parsed) != 1 || parsed == 0U) {
            (void)fprintf(stderr, "Repeat count must be positive.\n");
            exit_code = 1;
            goto finished;
        }
        status = umi_studio_test_service_discover_metadata(
            service, ".", "studio", argv[2], configuration,
            &discovery_summary);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_test_service_plan_all(
                service, (uint32_t)parsed, 0, &plan);
        }
        if (status == UMI_STATUS_OK) {
            status = umi_studio_test_service_execute(service, &plan,
                                                     &execution);
        } else {
            (void)memset(&execution, 0, sizeof(execution));
        }
        (void)printf("Planned: %zu\nExecuted: %zu\nPassed: %zu\n"
                     "Failed: %zu\nStopped: %s\nStatus: %s\n",
                     execution.planned, execution.executed, execution.passed,
                     execution.failed, execution.stopped ? "yes" : "no",
                     umi_status_text(status));
        exit_code = status == UMI_STATUS_OK && execution.failed == 0U ? 0 : 1;
    } else if (argc >= 2 && strcmp(argv[1], "--plan-all") == 0) {
        UmiTestPlatformOperationPlan plan;
        uint32_t repeat = 1U;
        if (argc >= 3) {
            unsigned parsed = 0U;
            if (sscanf(argv[2], "%u", &parsed) != 1 || parsed == 0U) {
                (void)fprintf(stderr, "Repeat count must be positive.\n");
                exit_code = 1;
                goto finished;
            }
            repeat = (uint32_t)parsed;
        }
        status = umi_studio_test_service_plan_all(service, repeat, 0, &plan);
        (void)printf("Selected: %zu\nExecutions: %zu\nStatus: %s\n",
                     plan.selection.count,
                     umi_test_platform_operation_execution_count(&plan),
                     umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc == 2 && strcmp(argv[1], "--rerun-failed") == 0) {
        UmiTestPlatformOperationPlan plan;
        status = umi_studio_test_service_plan_failed(service, &plan);
        (void)printf("Selected: %zu\nStatus: %s\n", plan.selection.count,
                     umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc == 2 && strcmp(argv[1], "--stop") == 0) {
        status = umi_studio_test_service_stop(service);
        (void)printf("Status: %s\n", umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc == 1 || (argc == 2 && strcmp(argv[1], "--status") == 0)) {
        status = umi_studio_test_service_snapshot(service, &snapshot);
        if (status == UMI_STATUS_OK) {
            (void)printf("Suites: %zu\nTests: %zu\nBuild directory: %s\n",
                         snapshot.suite_count,
                         snapshot.test_count,
                         snapshot.build_directory);
            (void)printf("Selected: %zu\nResults: %zu\nAttachments: %zu\n"
                         "Running: %s\nStop requested: %s\n",
                         snapshot.selected_count,
                         snapshot.retained_result_count,
                         snapshot.retained_attachment_count,
                         snapshot.operation_running ? "yes" : "no",
                         snapshot.stop_requested ? "yes" : "no");
        } else {
            exit_code = 1;
        }
    } else {
        (void)fprintf(
            stderr,
            "Usage:\n"
            "  umicom-studio-tests --discover <build-dir>\n"
            "  umicom-studio-tests --discover-metadata <build-dir> "
            "[configuration]\n"
            "  umicom-studio-tests --run\n"
            "  umicom-studio-tests --run-all <build-dir> [configuration]\n"
            "  umicom-studio-tests --repeat-all <build-dir> <count> "
            "[configuration]\n"
            "  umicom-studio-tests --plan-all [repeat-count]\n"
            "  umicom-studio-tests --rerun-failed\n"
            "  umicom-studio-tests --stop\n"
            "  umicom-studio-tests --status\n");
        exit_code = 2;
    }
finished:
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

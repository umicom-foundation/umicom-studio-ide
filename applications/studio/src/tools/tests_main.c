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
    } else if (argc == 2 && strcmp(argv[1], "--run") == 0) {
        UmiTestRunSummary summary;
        status = umi_studio_test_service_run_all(service, NULL, &summary);
        (void)printf("Passed: %zu\nFailed: %zu\nSkipped: %zu\n",
                     summary.passed, summary.failed, summary.skipped);
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else {
        status = umi_studio_test_service_snapshot(service, &snapshot);
        if (status == UMI_STATUS_OK) {
            (void)printf("Suites: %zu\nTests: %zu\nBuild directory: %s\n",
                         snapshot.suite_count,
                         snapshot.test_count,
                         snapshot.build_directory);
        } else {
            exit_code = 1;
        }
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

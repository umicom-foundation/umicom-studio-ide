/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/tests.c
 *
 * PURPOSE:
 *   Implement Studio test discovery, execution and aggregate test summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/tests.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioTestService {
    UmiTestRegistry *registry;
    UmiTestSuite *ctest_suite;
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    UmiTestRunSummary last_summary;
};

UmiStatus umi_studio_test_service_create(UmiStudioTestService **out_service)
{
    UmiStudioTestService *service;
    UmiStatus status;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiStudioTestService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_test_registry_create(&service->registry);
    if (status == UMI_STATUS_OK) {
        status = umi_test_suite_create("studio.ctest", "Studio CTest",
                                       &service->ctest_suite);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_test_registry_add(service->registry,
                                       service->ctest_suite);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_test_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_test_service_destroy(UmiStudioTestService *service)
{
    if (service == NULL) return;
    umi_test_registry_destroy(service->registry);
    free(service);
}

UmiStatus umi_studio_test_service_discover(UmiStudioTestService *service,
                                           const char *build_directory,
                                           size_t *out_discovered)
{
    size_t length;
    UmiStatus status;
    if (service == NULL || build_directory == NULL || build_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(build_directory);
    if (length + 1U > sizeof(service->build_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_test_registry_remove(service->registry, "studio.ctest");
    service->ctest_suite = NULL;
    status = umi_test_suite_create("studio.ctest", "Studio CTest",
                                   &service->ctest_suite);
    if (status == UMI_STATUS_OK) {
        status = umi_ctest_discover(build_directory,
                                    service->ctest_suite,
                                    out_discovered);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_test_registry_add(service->registry,
                                       service->ctest_suite);
    }
    if (status == UMI_STATUS_OK) {
        (void)memcpy(service->build_directory, build_directory, length + 1U);
    }
    return status;
}

UmiStatus umi_studio_test_service_run_all(UmiStudioTestService *service,
                                          UmiCancellationToken *cancellation,
                                          UmiTestRunSummary *out_summary)
{
    UmiTestResult *results;
    size_t count;
    UmiStatus status;
    if (service == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = umi_test_suite_count(service->ctest_suite);
    if (count == 0U) {
        (void)memset(out_summary, 0, sizeof(*out_summary));
        return UMI_STATUS_NOT_FOUND;
    }
    results = (UmiTestResult *)calloc(count, sizeof(*results));
    if (results == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_test_runner_run_suite(service->ctest_suite,
                                       cancellation,
                                       results,
                                       count,
                                       out_summary);
    service->last_summary = *out_summary;
    free(results);
    return status;
}

UmiStatus umi_studio_test_service_snapshot(
    const UmiStudioTestService *service,
    UmiStudioTestSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->build_directory,
                   sizeof(out_snapshot->build_directory),
                   "%s", service->build_directory);
    out_snapshot->suite_count = umi_test_registry_count(service->registry);
    out_snapshot->test_count = umi_test_suite_count(service->ctest_suite);
    out_snapshot->passed = service->last_summary.passed;
    out_snapshot->failed = service->last_summary.failed;
    out_snapshot->skipped = service->last_summary.skipped;
    out_snapshot->cancelled = service->last_summary.cancelled;
    out_snapshot->timed_out = service->last_summary.timed_out;
    return UMI_STATUS_OK;
}

UmiTestRegistry *umi_studio_test_service_registry(UmiStudioTestService *service)
{
    return service != NULL ? service->registry : NULL;
}

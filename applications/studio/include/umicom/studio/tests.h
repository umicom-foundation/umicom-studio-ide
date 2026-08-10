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
} UmiStudioTestSnapshot;

UmiStatus umi_studio_test_service_create(UmiStudioTestService **out_service);
void umi_studio_test_service_destroy(UmiStudioTestService *service);
UmiStatus umi_studio_test_service_discover(UmiStudioTestService *service,
                                           const char *build_directory,
                                           size_t *out_discovered);
UmiStatus umi_studio_test_service_run_all(UmiStudioTestService *service,
                                          UmiCancellationToken *cancellation,
                                          UmiTestRunSummary *out_summary);
UmiStatus umi_studio_test_service_snapshot(
    const UmiStudioTestService *service,
    UmiStudioTestSnapshot *out_snapshot
);
UmiTestRegistry *umi_studio_test_service_registry(
    UmiStudioTestService *service
);

#ifdef __cplusplus
}
#endif

#endif

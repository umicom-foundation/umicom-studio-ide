/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_observability.c
 *
 * PURPOSE:
 *   Verify Studio operational events, traces, profiler samples and readiness
 *   reporting through Framework observability services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/observability.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioObservabilityReport report;
    UmiProfileScope scope;
    uint64_t span_id = 0U;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_observability_begin(services,
                                          "studio.test",
                                          11U,
                                          0U,
                                          &scope,
                                          &span_id) == UMI_STATUS_OK);
    assert(umi_studio_observability_event(services,
                                          "studio.test",
                                          "test event",
                                          UMI_DIAGNOSTIC_INFO,
                                          11U) == UMI_STATUS_OK);
    assert(umi_studio_observability_end(services,
                                        &scope,
                                        span_id,
                                        UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(umi_studio_observability_report(services, &report) == UMI_STATUS_OK);
    assert(report.snapshot.metrics == 3U);
    assert(report.snapshot.spans == 1U);
    assert(report.profile_samples == 1U);
    assert(report.snapshot.operational_events >= 2U);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

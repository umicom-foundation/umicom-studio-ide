/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_services.c
 *
 * PURPOSE:
 *   Verify that Studio owns one Framework diagnostic hub, one retained
 *   diagnostic store, one typed settings repository, one clock, and optional
 *   presentation sinks through the explicit UmiStudioServices container.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/services.h"

#include <assert.h>
#include <stddef.h>

static void count_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    size_t *count = (size_t *)user_data;
    assert(diagnostic != NULL);
    assert(count != NULL);
    ++(*count);
}

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiDiagnostic diagnostic = {
        UMI_DIAGNOSTIC_INFO,
        "studio-services-test",
        "diagnostic fan-out",
        7U
    };
    UmiDiagnosticStoreSummary summary;
    UmiDiagnosticSink hub_sink;
    void *hub_user_data;
    size_t first_count = 0U;
    size_t second_count = 0U;

    assert(umi_studio_services_create(count_sink,
                                      &first_count,
                                      &services) == UMI_STATUS_OK);
    assert(services != NULL);

    /* The retained Framework store is always the first registered sink. */
    assert(umi_studio_services_diagnostic_sink_count(services) == 2U);
    assert(umi_studio_services_diagnostic_store(services) != NULL);
    assert(umi_studio_services_settings(services) != NULL);
    assert(umi_studio_services_clock(services) != NULL);

    assert(umi_studio_services_add_diagnostic_sink(services,
                                                   count_sink,
                                                   &second_count) == UMI_STATUS_OK);
    assert(umi_studio_services_diagnostic_sink_count(services) == 3U);

    hub_sink = umi_studio_services_diagnostic_sink();
    hub_user_data = umi_studio_services_diagnostic_user_data(services);
    assert(hub_sink != NULL);
    assert(hub_user_data != NULL);

    hub_sink(&diagnostic, hub_user_data);
    assert(first_count == 1U);
    assert(second_count == 1U);
    assert(umi_diagnostic_store_summary(
        umi_studio_services_diagnostic_store(services),
        &summary
    ) == UMI_STATUS_OK);
    assert(summary.retained_count == 1U);
    assert(summary.capacity == 512U);
    assert(summary.retained_by_severity[1] == 1U);

    assert(umi_studio_services_remove_diagnostic_sink(services,
                                                      count_sink,
                                                      &second_count) == UMI_STATUS_OK);
    assert(umi_studio_services_diagnostic_sink_count(services) == 2U);

    umi_studio_services_destroy(services);
    return 0;
}

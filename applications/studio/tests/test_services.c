/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_services.c
 *
 * PURPOSE:
 *   Verify that Studio uses the Framework diagnostic hub and clock through one
 *   explicit service container.
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
    UmiDiagnosticSink hub_sink;
    void *hub_user_data;
    size_t first_count = 0U;
    size_t second_count = 0U;

    assert(umi_studio_services_create(count_sink,
                                      &first_count,
                                      &services) == UMI_STATUS_OK);
    assert(services != NULL);
    assert(umi_studio_services_diagnostic_sink_count(services) == 1U);
    assert(umi_studio_services_add_diagnostic_sink(services,
                                                   count_sink,
                                                   &second_count) == UMI_STATUS_OK);
    assert(umi_studio_services_diagnostic_sink_count(services) == 2U);
    assert(umi_studio_services_clock(services) != NULL);

    hub_sink = umi_studio_services_diagnostic_sink();
    hub_user_data = umi_studio_services_diagnostic_user_data(services);
    assert(hub_sink != NULL);
    assert(hub_user_data != NULL);
    hub_sink(&diagnostic, hub_user_data);
    assert(first_count == 1U);
    assert(second_count == 1U);

    umi_studio_services_destroy(services);
    return 0;
}

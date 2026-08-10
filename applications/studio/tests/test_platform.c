/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_platform.c
 *
 * PURPOSE:
 *   Verify that Studio consumes the Framework toolchain and environment
 *   services without owning a duplicate discovery implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform.h"

#include <stdlib.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiStudioPlatformReport report;

    if (umi_studio_services_create(NULL, NULL, &services) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_studio_platform_check(services, 0, 0, &report) != UMI_STATUS_OK) {
        umi_studio_services_destroy(services);
        return EXIT_FAILURE;
    }
    if (!report.environment_ready ||
        !report.discovery.compile_probe_passed ||
        !report.discovery.link_probe_passed ||
        !report.discovery.runtime_probe_passed) {
        umi_studio_services_destroy(services);
        return EXIT_FAILURE;
    }
    umi_studio_platform_report_dispose(&report);
    umi_studio_services_destroy(services);
    return EXIT_SUCCESS;
}

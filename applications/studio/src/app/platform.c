/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/platform.c
 *
 * PURPOSE:
 *   Adapt the reusable Framework toolchain and environment services to Studio.
 *   The implementation deliberately delegates discovery, probing, dependency
 *   checks, and child environment construction to Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform.h"

#include <string.h>

UmiStatus umi_studio_platform_check(
    UmiStudioServices *services,
    int require_gtk,
    int require_github_cli,
    UmiStudioPlatformReport *out_report)
{
    UmiToolchainDiscoveryRequest request;
    UmiStatus status;

    if (services == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)memset(&request, 0, sizeof(request));
    request.require_gtk = require_gtk;
    request.require_github_cli = require_github_cli;
    request.diagnostic_sink = umi_studio_services_diagnostic_sink();
    request.diagnostic_user_data =
        umi_studio_services_diagnostic_user_data(services);

    status = umi_toolchain_discover(&request, &out_report->discovery);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    umi_environment_plan_init(&out_report->environment);
    status = umi_environment_plan_from_toolchain(
        &out_report->discovery.profile,
        &out_report->environment
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }
    out_report->framework_capability_count =
        umi_framework_capability_catalogue_count();
    out_report->environment_ready = 1;
    return UMI_STATUS_OK;
}

void umi_studio_platform_report_dispose(UmiStudioPlatformReport *report)
{
    if (report != NULL) {
        (void)memset(report, 0, sizeof(*report));
    }
}

UmiStatus umi_studio_platform_write_environment(
    const UmiStudioPlatformReport *report,
    const char *path)
{
    if (report == NULL || path == NULL || !report->environment_ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_environment_plan_write(&report->environment, path);
}

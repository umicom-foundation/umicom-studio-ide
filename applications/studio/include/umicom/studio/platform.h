/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/platform.h
 *
 * PURPOSE:
 *   Expose the Studio-facing view of Framework toolchain discovery, dependency
 *   validation, child-process environment construction, repository scaffolding,
 *   and reusable platform capability status without duplicating the Framework
 *   implementation inside the product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PLATFORM_H
#define UMICOM_STUDIO_PLATFORM_H

#include "umicom/umicom.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioPlatformReport {
    UmiToolchainDiscoveryReport discovery;
    UmiEnvironmentPlan environment;
    size_t framework_capability_count;
    int environment_ready;
} UmiStudioPlatformReport;

UmiStatus umi_studio_platform_check(
    UmiStudioServices *services,
    int require_gtk,
    int require_github_cli,
    UmiStudioPlatformReport *out_report
);
void umi_studio_platform_report_dispose(UmiStudioPlatformReport *report);
UmiStatus umi_studio_platform_write_environment(
    const UmiStudioPlatformReport *report,
    const char *path
);

#ifdef __cplusplus
}
#endif

#endif

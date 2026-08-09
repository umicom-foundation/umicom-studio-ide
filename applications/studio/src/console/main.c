/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/console/main.c
 *
 * PURPOSE:
 *   Provide a headless Studio frontend for validating Framework lifecycle,
 *   application versioning, module registration, and retained diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/diagnostics.h"
#include "umicom/studio/version.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDiagnosticStoreSummary summary;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);

    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Create failed: %s\n", umi_status_text(status));
        return 1;
    }

    status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Start failed: %s\n", umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }

    (void)printf("Application: %s\n",
                 umi_studio_bootstrap_application_name(bootstrap));
    (void)printf("Studio: %s\n", umi_studio_version_string());
    (void)printf("Framework: %s\n", UMICOM_FRAMEWORK_VERSION_STRING);
    (void)printf("Registered modules: %zu\n",
                 umi_studio_bootstrap_module_count(bootstrap));

    status = umi_studio_diagnostics_summary(
        umi_studio_bootstrap_services(bootstrap),
        &summary
    );
    if (status == UMI_STATUS_OK) {
        (void)printf("Retained diagnostics: %zu\n", summary.retained_count);
        (void)printf("Total diagnostics received: %" PRIu64 "\n",
                     summary.total_received);
    }

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

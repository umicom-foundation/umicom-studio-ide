/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/console/main.c
 *
 * PURPOSE:
 *   Provide a headless Studio frontend for validating Framework lifecycle,
 *   application versioning, module registration, typed settings, retained
 *   diagnostics, and Framework platform capability integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/diagnostics.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/platform.h"
#include "umicom/studio/settings.h"
#include "umicom/studio/version.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDiagnosticStoreSummary summary;
    UmiSettings *settings;
    char theme[UMI_SETTING_VALUE_CAPACITY];
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
    (void)printf("Framework capabilities: %zu\n",
                 umi_framework_capability_catalogue_count());
    (void)printf("Native command: umicom\n");

    settings = umi_studio_services_settings(
        umi_studio_bootstrap_services(bootstrap)
    );
    if (settings != NULL &&
        umi_settings_get_text(settings,
                              UMI_STUDIO_SETTING_UI_THEME,
                              theme,
                              sizeof(theme)) == UMI_STATUS_OK) {
        (void)printf("Typed settings: %zu\n", umi_settings_count(settings));
        (void)printf("Theme setting: %s\n", theme);
    }

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

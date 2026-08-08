/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/console/main.c
 *
 * PURPOSE:
 *   Provide a small console starter for Framework lifecycle validation.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/version.h"

#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
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

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

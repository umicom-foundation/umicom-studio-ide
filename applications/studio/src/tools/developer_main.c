/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/developer_main.c
 *
 * PURPOSE:
 *   Provide one native C23 summary command for all Framework-backed Studio developer services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/developer_platform.h"
#include "umicom/studio/services.h"

#include <stdio.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    char report[4096];
    UmiStatus status;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_developer_platform_report(
            umi_studio_services_developer_platform(
                umi_studio_bootstrap_services(bootstrap)),
            report,
            sizeof(report));
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("%s", report);
    } else {
        (void)fprintf(stderr, "Developer platform report failed: %s\n",
                      umi_status_text(status));
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/ui_catalogue_main.c
 *
 * PURPOSE:
 *   Print the Studio UI catalogue for architecture inspection and automated
 *   validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui_catalogue.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioUiCatalogue catalogue;
    char report[2048];
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ui_catalogue_snapshot(umi_studio_bootstrap_ui(bootstrap), &catalogue);
    }
    if (status == UMI_STATUS_OK) status = umi_studio_ui_catalogue_format(&catalogue, report, sizeof(report));
    if (status == UMI_STATUS_OK) (void)printf("%s", report);
    else (void)fprintf(stderr, "UI catalogue failed: %s\n", umi_status_text(status));
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

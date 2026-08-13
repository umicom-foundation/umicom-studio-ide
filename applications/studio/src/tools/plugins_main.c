/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/plugins_main.c
 *
 * PURPOSE:
 *   Discover and inspect Studio plug-in manifests without loading unapproved code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/plugins.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioPluginReport report;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);

    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Plug-in command failed: %s\n", umi_status_text(status));
        return 1;
    }
    status = argc >= 2
        ? umi_studio_plugins_discover(umi_studio_bootstrap_services(bootstrap),
                                      argv[1],
                                      1,
                                      &report)
        : umi_studio_plugins_report(umi_studio_bootstrap_services(bootstrap),
                                    &report);
    if (status == UMI_STATUS_OK) {
        (void)printf("Registered: %zu\nEnabled: %zu\nFailed: %zu\n"
                     "Contributions: %zu\nFiles examined: %zu\n"
                     "Manifests loaded: %zu\nInvalid manifests: %zu\n"
                     "Extension points: %zu\nCatalogue entries: %zu\n"
                     "Audit events: %zu\nPermission grants: %zu\n"
                     "Default isolation: %s\n",
                     report.registered, report.enabled, report.failed,
                     report.contributions, report.files_examined,
                     report.manifests_loaded, report.invalid_manifests,
                     report.extension_points, report.catalogue_entries,
                     report.audit_events, report.permission_grants,
                     report.default_isolation);
    } else {
        (void)fprintf(stderr, "Plug-in command failed: %s\n", umi_status_text(status));
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

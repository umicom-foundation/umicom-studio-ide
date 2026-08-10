/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/data_main.c
 *
 * PURPOSE:
 *   Provide a native Studio Data Server inspection and key/value command for integrity, migration and persistence checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/data.h"

#include <stdio.h>
#include <string.h>

static int fail(UmiStatus status)
{
    (void)fprintf(stderr, "Data command failed: %s\n", umi_status_text(status));
    return 1;
}

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) return fail(status);
    services = umi_studio_bootstrap_services(bootstrap);
    if (argc >= 4 && strcmp(argv[1], "set") == 0) {
        status = umi_studio_data_transaction_set(services, argv[2], argv[3]);
    } else if (argc >= 3 && strcmp(argv[1], "get") == 0) {
        char value[4096];
        status = umi_studio_data_get(services, argv[2], value, sizeof(value));
        if (status == UMI_STATUS_OK) (void)printf("%s\n", value);
    } else if (argc >= 3 && strcmp(argv[1], "remove") == 0) {
        status = umi_studio_data_remove(services, argv[2]);
    } else if (argc >= 2 && strcmp(argv[1], "integrity") == 0) {
        UmiStudioDataReport report;
        status = umi_studio_data_report(services, &report);
        if (status == UMI_STATUS_OK) {
            (void)printf("Backend: %s\nPath: %s\nRecords: %zu\nSchema: %u\nIntegrity: %s\n",
                         report.backend_name,
                         report.path[0] != '\0' ? report.path : "memory",
                         report.records,
                         report.schema_version,
                         umi_status_text(report.integrity_status));
        }
    } else {
        (void)printf("Usage: umicom-studio-data integrity | set KEY VALUE | get KEY | remove KEY\n");
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : fail(status);
}

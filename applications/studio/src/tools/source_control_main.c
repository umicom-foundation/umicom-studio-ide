/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/source_control_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for inspecting Studio source-control status and history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/services.h"
#include "umicom/studio/source_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioSourceControlService *service;
    UmiStudioSourceControlSnapshot snapshot;
    UmiStatus status;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Studio source-control startup failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    service = umi_studio_services_source_control(
        umi_studio_bootstrap_services(bootstrap));
    if (argc >= 2 && strcmp(argv[1], "--refresh") == 0) {
        size_t limit = argc >= 3 ? (size_t)strtoul(argv[2], NULL, 10) : 20U;
        status = umi_studio_source_control_service_refresh(service, limit);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_UNAVAILABLE) {
            exit_code = 1;
        }
    }
    if (umi_studio_source_control_service_snapshot(service, &snapshot) ==
        UMI_STATUS_OK) {
        (void)printf("Provider: %s\nAvailable: %s\nBranch: %s\n"
                     "Upstream: %s\nAhead: %d\nBehind: %d\n"
                     "Changes: %zu\nStaged: %zu\nCommits: %zu\n",
                     snapshot.provider_id,
                     snapshot.available ? "yes" : "no",
                     snapshot.branch,
                     snapshot.upstream,
                     snapshot.ahead,
                     snapshot.behind,
                     snapshot.changes,
                     snapshot.staged,
                     snapshot.commits);
    } else {
        exit_code = 1;
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

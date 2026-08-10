/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/language_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for Studio Language Server Protocol state and requests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/language.h"
#include "umicom/studio/services.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioLanguageService *service;
    UmiStudioLanguageSnapshot snapshot;
    UmiStatus status;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Studio language service startup failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    service = umi_studio_services_language(
        umi_studio_bootstrap_services(bootstrap));
    if (argc == 2 && strcmp(argv[1], "--initialize") == 0) {
        int64_t request_id = 0;
        status = umi_studio_language_service_initialize(service, 0, &request_id);
        (void)printf("Initialise request: %lld\nStatus: %s\n",
                     (long long)request_id, umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else if (argc == 5 && strcmp(argv[1], "--open") == 0) {
        status = umi_studio_language_service_open(service,
                                                  argv[2],
                                                  argv[3],
                                                  1,
                                                  argv[4]);
        (void)printf("Open document: %s\n", umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else {
        status = umi_studio_language_service_snapshot(service, &snapshot);
        if (status == UMI_STATUS_OK) {
            (void)printf("Client state: %d\nInitialized: %s\n"
                         "Open documents: %zu\nQueued messages: %zu\n",
                         (int)snapshot.client_state,
                         snapshot.initialized ? "yes" : "no",
                         snapshot.open_documents,
                         snapshot.queued_messages);
        } else {
            exit_code = 1;
        }
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

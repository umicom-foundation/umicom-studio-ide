/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/terminal_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for prepared Studio terminal sessions and execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/services.h"
#include "umicom/studio/terminal.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioTerminalService *service;
    UmiStudioTerminalSnapshot snapshot;
    UmiStatus status;
    int command_exit = 0;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Studio terminal startup failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    service = umi_studio_services_terminal(
        umi_studio_bootstrap_services(bootstrap));
    if (argc == 3 && strcmp(argv[1], "--execute") == 0) {
        status = umi_studio_terminal_service_execute(service,
                                                     argv[2],
                                                     30000U,
                                                     NULL,
                                                     &command_exit);
        (void)printf("Command exit code: %d\nStatus: %s\n",
                     command_exit, umi_status_text(status));
        exit_code = status == UMI_STATUS_OK ? 0 : 1;
    } else {
        status = umi_studio_terminal_service_snapshot(service, &snapshot);
        if (status == UMI_STATUS_OK) {
            (void)printf("Sessions: %zu\nPrimary: %s\nWorking directory: %s\n"
                         "Commands executed: %llu\nTranscript lines: %zu\n",
                         snapshot.session_count,
                         snapshot.primary.title,
                         snapshot.primary.working_directory,
                         (unsigned long long)snapshot.primary.commands_executed,
                         snapshot.primary.transcript_lines);
        } else {
            exit_code = 1;
        }
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

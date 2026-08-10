/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/state_main.c
 *
 * PURPOSE:
 *   Provide a native C23 operational-state command for inspecting Studio's
 *   Framework registries and executing canonical Studio commands without GTK4.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/state.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStatus status;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Could not create Studio: %s\n",
                      umi_status_text(status));
        return 1;
    }

    status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Could not start Studio: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }

    if (argc >= 3 && strcmp(argv[1], "--execute") == 0) {
        char message[512];
        const char *argument = argc >= 4 ? argv[3] : "";
        status = umi_command_registry_execute(
            umi_studio_bootstrap_command_registry(bootstrap),
            argv[2],
            argument,
            message,
            sizeof(message)
        );
        if (message[0] != '\0') {
            (void)printf("%s\n", message);
        }
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr,
                          "Command failed: %s\n",
                          umi_status_text(status));
            exit_code = 1;
        }
    } else {
        UmiStudioStateReport report;
        char text[1024];

        status = umi_studio_state_capture(bootstrap, &report);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_state_format(&report,
                                             text,
                                             sizeof(text));
        }
        if (status == UMI_STATUS_OK) {
            (void)printf("%s", text);
        } else {
            (void)fprintf(stderr,
                          "Could not capture Studio state: %s\n",
                          umi_status_text(status));
            exit_code = 1;
        }
    }

    if (umi_studio_bootstrap_stop(bootstrap) != UMI_STATUS_OK) {
        exit_code = 1;
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

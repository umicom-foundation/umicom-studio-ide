/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/process_main.c
 *
 * PURPOSE:
 *   Run one trusted-workspace child process through the Framework supervisor
 *   and print its final state, exit code, duration and captured output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/processes.h"
#include "umicom/studio/workspace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void)
{
    (void)fprintf(stderr,
                  "Usage: umicom-studio-process ROOT [--timeout MS] -- PROGRAM [ARGS...]\n");
}

int main(int argc, char **argv)
{
    UmiStudioServices *services = NULL;
    UmiProcessRequest request;
    UmiProcessJobSnapshot snapshot;
    UmiProcessJobId job_id = 0U;
    UmiStatus status;
    uint32_t timeout_ms = 30000U;
    int separator = -1;
    int index;

    if (argc < 4) {
        usage();
        return 2;
    }
    for (index = 2; index < argc; ++index) {
        if (strcmp(argv[index], "--") == 0) {
            separator = index;
            break;
        }
        if (strcmp(argv[index], "--timeout") == 0 && index + 1 < argc) {
            timeout_ms = (uint32_t)strtoul(argv[++index], NULL, 10);
        }
    }
    if (separator < 0 || separator + 1 >= argc) {
        usage();
        return 2;
    }

    status = umi_studio_services_create(NULL, NULL, &services);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_workspace_open(services, argv[1], 1, 0);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Workspace failed: %s\n", umi_status_text(status));
        umi_studio_services_destroy(services);
        return 1;
    }

    (void)memset(&request, 0, sizeof(request));
    request.program = argv[separator + 1];
    request.arguments = (const char *const *)&argv[separator + 2];
    request.argument_count = (size_t)(argc - separator - 2);
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = timeout_ms;
    request.poll_interval_ms = 10U;

    status = umi_studio_process_submit(services,
                                       "studio-process-command",
                                       &request,
                                       &job_id);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_process_wait(services, job_id, timeout_ms + 1000U);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_process_snapshot(services, job_id, &snapshot);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Process failed: %s\n", umi_status_text(status));
        umi_studio_services_destroy(services);
        return 1;
    }

    (void)printf("State: %s\n", umi_process_job_state_text(snapshot.state));
    (void)printf("Exit code: %d\n", snapshot.exit_code);
    (void)printf("Duration: %llu ms\n",
                 (unsigned long long)snapshot.duration_ms);
    if (snapshot.output[0] != '\0') (void)printf("%s", snapshot.output);

    umi_studio_services_destroy(services);
    return snapshot.state == UMI_PROCESS_JOB_SUCCEEDED ? 0 : 1;
}

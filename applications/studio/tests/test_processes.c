/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_processes.c
 *
 * PURPOSE:
 *   Verify trusted-workspace process submission, waiting, output capture and
 *   rejection when a workspace is untrusted.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/processes.h"
#include "umicom/studio/workspace.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiProcessRequest request;
    UmiProcessJobSnapshot snapshot;
    UmiProcessJobId job_id = 0U;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
#ifdef _WIN32
    const char *arguments[] = {"/C", "echo studio-process"};
    const char *program = "cmd.exe";
#else
    const char *arguments[] = {"-c", "printf studio-process"};
    const char *program = "/bin/sh";
#endif

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_path_join(temporary,
                         "umicom-studio-process-test",
                         root,
                         sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_workspace_open(services, root, 1, 0) == UMI_STATUS_OK);

    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = arguments;
    request.argument_count = 2U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = 5000U;
    request.poll_interval_ms = 10U;
    assert(umi_studio_process_submit(services,
                                     "test-process",
                                     &request,
                                     &job_id) == UMI_STATUS_OK);
    assert(umi_studio_process_wait(services, job_id, 6000U) == UMI_STATUS_OK);
    assert(umi_studio_process_snapshot(services, job_id, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_PROCESS_JOB_SUCCEEDED);
    assert(strstr(snapshot.output, "studio-process") != NULL);
    assert(umi_studio_process_stats(services).succeeded == 1U);

    assert(umi_studio_workspace_set_trusted(services, 0) == UMI_STATUS_OK);
    assert(umi_studio_process_submit(services,
                                     "blocked-process",
                                     &request,
                                     &job_id) == UMI_STATUS_PERMISSION_DENIED);

    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}

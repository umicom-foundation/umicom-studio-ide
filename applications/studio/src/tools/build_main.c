/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/build_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for inspecting and executing Studio build profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/build.h"
#include "umicom/studio/services.h"

#include <stdio.h>
#include <string.h>

static int parse_phase(const char *value, UmiBuildPhase *out_phase)
{
    if (strcmp(value, "configure") == 0) *out_phase = UMI_BUILD_PHASE_CONFIGURE;
    else if (strcmp(value, "build") == 0) *out_phase = UMI_BUILD_PHASE_BUILD;
    else if (strcmp(value, "test") == 0) *out_phase = UMI_BUILD_PHASE_TEST;
    else if (strcmp(value, "clean") == 0) *out_phase = UMI_BUILD_PHASE_CLEAN;
    else if (strcmp(value, "run") == 0) *out_phase = UMI_BUILD_PHASE_RUN;
    else return 0;
    return 1;
}

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioBuildService *service;
    UmiStudioBuildSnapshot snapshot;
    UmiBuildResult result;
    UmiStatus status;
    int exit_code = 0;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Studio build service startup failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }
    service = umi_studio_services_build(
        umi_studio_bootstrap_services(bootstrap));

    if (argc == 3 && strcmp(argv[1], "--phase") == 0) {
        UmiBuildPhase phase;
        if (!parse_phase(argv[2], &phase)) {
            (void)fprintf(stderr, "Unknown build phase: %s\n", argv[2]);
            exit_code = 1;
        } else {
            status = umi_studio_build_service_run(service, phase, &result);
            (void)printf("%s: %s\nExit code: %d\nDiagnostics: %zu\n",
                         umi_build_phase_text(phase),
                         umi_status_text(status),
                         result.exit_code,
                         result.diagnostics.count);
            exit_code = status == UMI_STATUS_OK ? 0 : 1;
        }
    } else {
        status = umi_studio_build_service_snapshot(service, &snapshot);
        if (status == UMI_STATUS_OK) {
            (void)printf("Build profile: %s\nSource: %s\nBuild: %s\n"
                         "History: %zu\nNext operation: %llu\n",
                         snapshot.profile_id,
                         snapshot.source_root,
                         snapshot.build_directory,
                         snapshot.history_count,
                         (unsigned long long)snapshot.next_operation_id);
        } else {
            exit_code = 1;
        }
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}

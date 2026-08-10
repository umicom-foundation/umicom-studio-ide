/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/build.c
 *
 * PURPOSE:
 *   Implement Studio build profiles, execution, diagnostic capture and retained history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioBuildService {
    UmiBuildProfile profile;
    UmiBuildHistory *history;
    UmiBuildRunner *runner;
    UmiCancellationToken *cancellation;
    UmiBuildResult last_result;
    int has_last_result;
};

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_build_service_create(const char *source_root,
                                           UmiClock *clock,
                                           UmiStudioBuildService **out_service)
{
    UmiStudioBuildService *service;
    UmiBuildRunnerConfig config;
    UmiStatus status;

    if (source_root == NULL || source_root[0] == '\0' || clock == NULL ||
        out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiStudioBuildService *)calloc(1U, sizeof(*service));
    if (service == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_build_profile_init(&service->profile);
    status = umi_build_profile_set(&service->profile,
                                   "studio.development",
                                   source_root,
                                   "build/umicom-development");
    if (status == UMI_STATUS_OK) {
        status = copy_text(service->profile.generator,
                           sizeof(service->profile.generator),
                           "Ninja");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(service->profile.configuration,
                           sizeof(service->profile.configuration),
                           "Debug");
    }
    service->profile.parallel_jobs = 4U;
    service->profile.timeout_ms = 0U;
    service->profile.build_testing = 1;
    service->profile.strict_warnings = 1;
    if (status == UMI_STATUS_OK) {
        status = umi_build_history_create(64U, &service->history);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_cancellation_token_create(&service->cancellation);
    }
    if (status == UMI_STATUS_OK) {
        (void)memset(&config, 0, sizeof(config));
        config.profile = service->profile;
        config.history = service->history;
        config.clock = clock;
        config.cancellation = service->cancellation;
        status = umi_build_runner_create(&config, &service->runner);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_build_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_build_service_destroy(UmiStudioBuildService *service)
{
    if (service == NULL) return;
    umi_build_runner_destroy(service->runner);
    umi_cancellation_token_destroy(service->cancellation);
    umi_build_history_destroy(service->history);
    free(service);
}

UmiStatus umi_studio_build_service_set_profile(
    UmiStudioBuildService *service,
    const UmiBuildProfile *profile)
{
    UmiStatus status;
    char message[256];
    if (service == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_profile_validate(profile, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_runner_set_profile(service->runner, profile);
    if (status == UMI_STATUS_OK) service->profile = *profile;
    return status;
}

UmiStatus umi_studio_build_service_run(UmiStudioBuildService *service,
                                       UmiBuildPhase phase,
                                       UmiBuildResult *out_result)
{
    UmiStatus status;
    if (service == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_runner_run(service->runner, phase, out_result);
    service->last_result = *out_result;
    service->has_last_result = 1;
    return status;
}

UmiStatus umi_studio_build_service_snapshot(
    const UmiStudioBuildService *service,
    UmiStudioBuildSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)copy_text(out_snapshot->source_root,
                    sizeof(out_snapshot->source_root),
                    service->profile.source_directory);
    (void)copy_text(out_snapshot->build_directory,
                    sizeof(out_snapshot->build_directory),
                    service->profile.build_directory);
    (void)copy_text(out_snapshot->profile_id,
                    sizeof(out_snapshot->profile_id),
                    service->profile.profile_id);
    out_snapshot->history_count = umi_build_history_count(service->history);
    out_snapshot->next_operation_id =
        umi_build_runner_next_operation_id(service->runner);
    if (service->has_last_result) {
        out_snapshot->last_phase = service->last_result.phase;
        out_snapshot->last_state = service->last_result.state;
        out_snapshot->last_status = service->last_result.status;
        out_snapshot->last_exit_code = service->last_result.exit_code;
        out_snapshot->diagnostic_count =
            service->last_result.diagnostics.count;
    }
    return UMI_STATUS_OK;
}

UmiBuildHistory *umi_studio_build_service_history(UmiStudioBuildService *service)
{
    return service != NULL ? service->history : NULL;
}

const UmiBuildProfile *umi_studio_build_service_profile(
    const UmiStudioBuildService *service)
{
    return service != NULL ? &service->profile : NULL;
}

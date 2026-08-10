/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/source_control.c
 *
 * PURPOSE:
 *   Implement Studio source-control state over the provider-neutral Framework VCS boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/source_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioSourceControlService {
    char root[UMI_VCS_PATH_CAPACITY];
    UmiVcsProvider provider;
    UmiVcsRepository *repository;
    UmiVcsChangeList *changes;
    UmiVcsHistory *history;
    UmiVcsBranch branch;
    int available;
};

UmiStatus umi_studio_source_control_service_create(
    const char *root,
    UmiStudioSourceControlService **out_service)
{
    UmiStudioSourceControlService *service;
    UmiStatus status;
    size_t length;
    int is_repository = 0;
    if (root == NULL || root[0] == '\0' || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    length = strlen(root);
    if (length + 1U > UMI_VCS_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    service = (UmiStudioSourceControlService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(service->root, root, length + 1U);
    status = umi_vcs_change_list_create(&service->changes);
    if (status == UMI_STATUS_OK) status = umi_vcs_history_create(&service->history);
    if (status == UMI_STATUS_OK) status = umi_vcs_git_cli_provider(&service->provider);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_git_cli_is_repository(root, &is_repository);
        if (status != UMI_STATUS_OK) {
            status = UMI_STATUS_OK;
            is_repository = 0;
        }
    }
    service->available = is_repository;
    if (status == UMI_STATUS_OK && service->available) {
        status = umi_vcs_repository_create(root,
                                           &service->provider,
                                           &service->repository);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_source_control_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_source_control_service_destroy(
    UmiStudioSourceControlService *service)
{
    if (service == NULL) return;
    if (service->repository != NULL) {
        /* The repository owns the copied provider instance. */
        umi_vcs_repository_destroy(service->repository);
    } else if (service->provider.destroy != NULL) {
        service->provider.destroy(service->provider.instance);
    }
    umi_vcs_history_destroy(service->history);
    umi_vcs_change_list_destroy(service->changes);
    free(service);
}

UmiStatus umi_studio_source_control_service_refresh(
    UmiStudioSourceControlService *service,
    size_t history_limit)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!service->available || service->repository == NULL) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_vcs_repository_status(service->repository,
                                       service->changes,
                                       &service->branch);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_repository_history(service->repository,
                                            history_limit,
                                            service->history);
    }
    return status;
}

UmiStatus umi_studio_source_control_service_stage(
    UmiStudioSourceControlService *service,
    const char *path)
{
    if (service == NULL || service->repository == NULL) return UMI_STATUS_UNAVAILABLE;
    return umi_vcs_repository_stage(service->repository, path);
}

UmiStatus umi_studio_source_control_service_unstage(
    UmiStudioSourceControlService *service,
    const char *path)
{
    if (service == NULL || service->repository == NULL) return UMI_STATUS_UNAVAILABLE;
    return umi_vcs_repository_unstage(service->repository, path);
}

UmiStatus umi_studio_source_control_service_commit(
    UmiStudioSourceControlService *service,
    const char *message,
    char *out_commit_id,
    size_t capacity)
{
    if (service == NULL || service->repository == NULL) return UMI_STATUS_UNAVAILABLE;
    return umi_vcs_repository_commit(service->repository,
                                     message,
                                     out_commit_id,
                                     capacity);
}

UmiStatus umi_studio_source_control_service_snapshot(
    const UmiStudioSourceControlService *service,
    UmiStudioSourceControlSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->root, sizeof(out_snapshot->root),
                   "%s", service->root);
    (void)snprintf(out_snapshot->provider_id,
                   sizeof(out_snapshot->provider_id), "%s",
                   service->provider.provider_id != NULL
                       ? service->provider.provider_id : "unavailable");
    (void)snprintf(out_snapshot->branch, sizeof(out_snapshot->branch),
                   "%s", service->branch.name);
    (void)snprintf(out_snapshot->upstream, sizeof(out_snapshot->upstream),
                   "%s", service->branch.upstream);
    out_snapshot->available = service->available;
    out_snapshot->ahead = service->branch.ahead;
    out_snapshot->behind = service->branch.behind;
    out_snapshot->changes = umi_vcs_change_list_count(service->changes);
    out_snapshot->staged = umi_vcs_change_list_staged_count(service->changes);
    out_snapshot->commits = umi_vcs_history_count(service->history);
    return UMI_STATUS_OK;
}

const UmiVcsChangeList *umi_studio_source_control_service_changes(
    const UmiStudioSourceControlService *service)
{
    return service != NULL ? service->changes : NULL;
}

const UmiVcsHistory *umi_studio_source_control_service_history(
    const UmiStudioSourceControlService *service)
{
    return service != NULL ? service->history : NULL;
}

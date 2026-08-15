/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/source_control.h
 *
 * PURPOSE:
 *   Expose provider-neutral source-control status, history and Git operations to Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_SOURCE_CONTROL_H
#define UMICOM_STUDIO_SOURCE_CONTROL_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSourceControlService UmiStudioSourceControlService;

typedef struct UmiStudioSourceControlSnapshot {
    char root[UMI_VCS_PATH_CAPACITY];
    char provider_id[UMI_VCS_ID_CAPACITY];
    char branch[UMI_VCS_NAME_CAPACITY];
    char upstream[UMI_VCS_NAME_CAPACITY];
    int available;
    int ahead;
    int behind;
    size_t changes;
    size_t staged;
    size_t commits;
    size_t conflicts;
    size_t branches;
    size_t remotes;
    size_t tags;
    size_t operations;
    uint64_t revision;
    uint64_t capabilities;
} UmiStudioSourceControlSnapshot;

UmiStatus umi_studio_source_control_service_create(
    const char *root,
    UmiStudioSourceControlService **out_service
);
void umi_studio_source_control_service_destroy(
    UmiStudioSourceControlService *service
);
UmiStatus umi_studio_source_control_service_refresh(
    UmiStudioSourceControlService *service,
    size_t history_limit
);
UmiStatus umi_studio_source_control_service_stage(
    UmiStudioSourceControlService *service,
    const char *path
);
UmiStatus umi_studio_source_control_service_unstage(
    UmiStudioSourceControlService *service,
    const char *path
);
UmiStatus umi_studio_source_control_service_stage_all(UmiStudioSourceControlService *service);
UmiStatus umi_studio_source_control_service_unstage_all(UmiStudioSourceControlService *service);
UmiStatus umi_studio_source_control_service_discard(UmiStudioSourceControlService *service, const char *path);
UmiStatus umi_studio_source_control_service_commit(
    UmiStudioSourceControlService *service,
    const char *message,
    char *out_commit_id,
    size_t capacity
);
UmiStatus umi_studio_source_control_service_fetch(UmiStudioSourceControlService *service);
UmiStatus umi_studio_source_control_service_pull(UmiStudioSourceControlService *service);
UmiStatus umi_studio_source_control_service_push(UmiStudioSourceControlService *service);
UmiStatus umi_studio_source_control_service_branch_create(UmiStudioSourceControlService *service, const char *name, int checkout);
UmiStatus umi_studio_source_control_service_branch_checkout(UmiStudioSourceControlService *service, const char *name);
UmiStatus umi_studio_source_control_service_branch_delete(UmiStudioSourceControlService *service, const char *name, int force);
UmiStatus umi_studio_source_control_service_open_diff(UmiStudioSourceControlService *service, const char *path, int staged);
UmiStatus umi_studio_source_control_service_snapshot(
    const UmiStudioSourceControlService *service,
    UmiStudioSourceControlSnapshot *out_snapshot
);
const UmiVcsChangeList *umi_studio_source_control_service_changes(
    const UmiStudioSourceControlService *service
);
const UmiVcsHistory *umi_studio_source_control_service_history(
    const UmiStudioSourceControlService *service
);
UmiVcsWorkspace *umi_studio_source_control_service_workspace(UmiStudioSourceControlService *service);
UmiVcsWorkspaceCoordinator *umi_studio_source_control_service_coordinator(
    UmiStudioSourceControlService *service
);

#ifdef __cplusplus
}
#endif

#endif

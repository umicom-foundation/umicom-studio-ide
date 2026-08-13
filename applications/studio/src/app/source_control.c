/*-----------------------------------------------------------------------------
 * Umicom Studio source-control facade.
 * Studio owns no repository algorithms or collections. It delegates to the
 * reusable Framework workspace shared by every Umicom application.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/studio/source_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiStudioSourceControlService { UmiSourceControlService *foundation; };
UmiStatus umi_studio_source_control_service_create(const char *root, UmiStudioSourceControlService **out_service)
{
    UmiStudioSourceControlService *service; UmiStatus status;
    if (root == NULL || root[0] == '\0' || out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL; service = calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_source_control_service_create(&service->foundation);
    if (status == UMI_STATUS_OK) {
        status = umi_source_control_service_open_workspace(service->foundation, root);
    }
    if (status != UMI_STATUS_OK) { free(service); return status; }
    *out_service = service; return UMI_STATUS_OK;
}
void umi_studio_source_control_service_destroy(UmiStudioSourceControlService *service)
{ if (service != NULL) { umi_source_control_service_destroy(service->foundation); free(service); } }
UmiStatus umi_studio_source_control_service_refresh(UmiStudioSourceControlService *service, size_t history_limit)
{ return service != NULL ? umi_vcs_workspace_refresh(umi_source_control_service_workspace(service->foundation), history_limit) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_stage(UmiStudioSourceControlService *service, const char *path)
{ return service != NULL ? umi_vcs_workspace_stage(umi_source_control_service_workspace(service->foundation), path) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_unstage(UmiStudioSourceControlService *service, const char *path)
{ return service != NULL ? umi_vcs_workspace_unstage(umi_source_control_service_workspace(service->foundation), path) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_stage_all(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_stage_all(umi_source_control_service_workspace(service->foundation)) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_unstage_all(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_unstage_all(umi_source_control_service_workspace(service->foundation)) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_discard(UmiStudioSourceControlService *service, const char *path)
{ return service != NULL ? umi_vcs_workspace_discard(umi_source_control_service_workspace(service->foundation), path) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_commit(UmiStudioSourceControlService *service, const char *message, char *out_commit_id, size_t capacity)
{ return service != NULL ? umi_vcs_workspace_commit(umi_source_control_service_workspace(service->foundation), message, out_commit_id, capacity) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_fetch(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_fetch(umi_source_control_service_workspace(service->foundation)) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_pull(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_pull(umi_source_control_service_workspace(service->foundation)) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_push(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_push(umi_source_control_service_workspace(service->foundation)) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_branch_create(UmiStudioSourceControlService *service, const char *name, int checkout)
{ return service != NULL ? umi_vcs_workspace_branch_create(umi_source_control_service_workspace(service->foundation), name, checkout) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_branch_checkout(UmiStudioSourceControlService *service, const char *name)
{ return service != NULL ? umi_vcs_workspace_branch_checkout(umi_source_control_service_workspace(service->foundation), name) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_branch_delete(UmiStudioSourceControlService *service, const char *name, int force)
{ return service != NULL ? umi_vcs_workspace_branch_delete(umi_source_control_service_workspace(service->foundation), name, force) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_open_diff(UmiStudioSourceControlService *service, const char *path, int staged)
{ return service != NULL ? umi_vcs_workspace_open_diff(umi_source_control_service_workspace(service->foundation), path, staged) : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus umi_studio_source_control_service_snapshot(const UmiStudioSourceControlService *service, UmiStudioSourceControlSnapshot *out_snapshot)
{
    UmiVcsWorkspaceSnapshot source;
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_workspace_snapshot(umi_source_control_service_workspace_const(service->foundation), &source) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->root, sizeof(out_snapshot->root), "%s", source.root);
    (void)snprintf(out_snapshot->provider_id, sizeof(out_snapshot->provider_id), "%s", source.provider_id);
    (void)snprintf(out_snapshot->branch, sizeof(out_snapshot->branch), "%s", source.branch);
    (void)snprintf(out_snapshot->upstream, sizeof(out_snapshot->upstream), "%s", source.upstream);
    out_snapshot->available = source.available; out_snapshot->ahead = source.ahead; out_snapshot->behind = source.behind;
    out_snapshot->changes = source.changes; out_snapshot->staged = source.staged; out_snapshot->commits = source.commits;
    out_snapshot->conflicts = source.conflicts; out_snapshot->branches = source.branches; out_snapshot->remotes = source.remotes;
    out_snapshot->tags = source.tags; out_snapshot->operations = source.operations; out_snapshot->revision = source.revision;
    out_snapshot->capabilities = source.capabilities;
    return UMI_STATUS_OK;
}
const UmiVcsChangeList *umi_studio_source_control_service_changes(const UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_changes(umi_source_control_service_workspace_const(service->foundation)) : NULL; }
const UmiVcsHistory *umi_studio_source_control_service_history(const UmiStudioSourceControlService *service)
{ return service != NULL ? umi_vcs_workspace_history(umi_source_control_service_workspace_const(service->foundation)) : NULL; }
UmiVcsWorkspace *umi_studio_source_control_service_workspace(UmiStudioSourceControlService *service)
{ return service != NULL ? umi_source_control_service_workspace(service->foundation) : NULL; }

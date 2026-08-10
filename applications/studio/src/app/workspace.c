/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace.c
 *
 * PURPOSE:
 *   Coordinate workspace opening, project discovery, index refresh, trust and
 *   watcher lifecycle through the shared Framework service container.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace.h"

#include "umicom/studio/watcher.h"

UmiStatus umi_studio_workspace_open(UmiStudioServices *services,
                                    const char *root,
                                    int trusted,
                                    int start_watcher)
{
    UmiStatus status;
    if (services == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_services_open_workspace(services, root, trusted);
    if (status == UMI_STATUS_OK && start_watcher) {
        status = umi_studio_watcher_start(services);
    }
    return status;
}

UmiStatus umi_studio_workspace_close(UmiStudioServices *services)
{
    return umi_studio_services_close_workspace(services);
}

UmiStatus umi_studio_workspace_refresh(UmiStudioServices *services)
{
    UmiStatus status;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workspace_graph_discover(
        umi_studio_services_workspace(services));
    if (status == UMI_STATUS_OK) {
        status = umi_file_index_rebuild(
            umi_studio_services_file_index(services));
    }
    return status;
}

UmiStatus umi_studio_workspace_set_trusted(UmiStudioServices *services,
                                           int trusted)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workspace_graph_set_trusted(
        umi_studio_services_workspace(services), trusted);
}

UmiStatus umi_studio_workspace_snapshot(
    UmiStudioServices *services,
    UmiStudioWorkspaceSnapshot *out_snapshot)
{
    UmiStatus status;
    if (services == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workspace_graph_snapshot(
        umi_studio_services_workspace(services),
        &out_snapshot->graph);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->files = umi_file_index_stats(
        umi_studio_services_file_index(services));
    out_snapshot->watcher = umi_watcher_stats(
        umi_studio_services_watcher(services));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_workspace_project_at(
    UmiStudioServices *services,
    size_t index,
    UmiWorkspaceProjectSnapshot *out_project)
{
    if (services == NULL || out_project == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workspace_graph_project_at(
        umi_studio_services_workspace(services), index, out_project);
}

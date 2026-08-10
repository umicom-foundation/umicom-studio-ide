/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/processes.c
 *
 * PURPOSE:
 *   Enforce Studio workspace trust and delegate asynchronous child-process
 *   lifecycle to the shared Framework process supervisor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/processes.h"

UmiStatus umi_studio_process_submit(UmiStudioServices *services,
                                    const char *label,
                                    const UmiProcessRequest *request,
                                    UmiProcessJobId *out_job_id)
{
    UmiWorkspaceGraphSnapshot workspace;
    UmiProcessRequest effective;
    UmiStatus status;
    if (services == NULL || request == NULL || out_job_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workspace_graph_snapshot(
        umi_studio_services_workspace(services), &workspace);
    if (status != UMI_STATUS_OK || !workspace.open) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }
    if (!workspace.trusted) return UMI_STATUS_PERMISSION_DENIED;
    effective = *request;
    if (effective.working_directory == NULL ||
        effective.working_directory[0] == '\0') {
        effective.working_directory = workspace.root;
    }
    return umi_process_supervisor_submit(
        umi_studio_services_process_supervisor(services),
        label,
        &effective,
        out_job_id);
}

UmiStatus umi_studio_process_cancel(UmiStudioServices *services,
                                    UmiProcessJobId job_id)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_supervisor_cancel(
        umi_studio_services_process_supervisor(services), job_id);
}

UmiStatus umi_studio_process_wait(UmiStudioServices *services,
                                  UmiProcessJobId job_id,
                                  uint32_t timeout_ms)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_supervisor_wait(
        umi_studio_services_process_supervisor(services),
        job_id,
        timeout_ms);
}

UmiStatus umi_studio_process_snapshot(
    UmiStudioServices *services,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_supervisor_snapshot(
        umi_studio_services_process_supervisor(services),
        job_id,
        out_snapshot);
}

UmiProcessSupervisorStats umi_studio_process_stats(
    UmiStudioServices *services)
{
    return services != NULL
        ? umi_process_supervisor_stats(
              umi_studio_services_process_supervisor(services))
        : (UmiProcessSupervisorStats){0};
}

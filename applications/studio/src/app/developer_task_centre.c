/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_task_centre.c
 *
 * PURPOSE:
 *   Implement Studio task browsing and task-to-pipeline submission.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_task_centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioDeveloperTaskCentre {
    UmiDeveloperRuntime *runtime;
    uint64_t revision;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiProjectTaskRegistry *task_registry(UmiStudioDeveloperTaskCentre *centre)
{
    if (centre == NULL || centre->runtime == NULL) return NULL;
    return umi_project_workspace_task(umi_developer_runtime_projects(centre->runtime));
}

UmiStatus umi_studio_developer_task_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperTaskCentre **out_centre)
{
    UmiStudioDeveloperTaskCentre *centre;
    if (runtime == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioDeveloperTaskCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->runtime = runtime;
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_developer_task_centre_destroy(UmiStudioDeveloperTaskCentre *centre)
{
    free(centre);
}

UmiStatus umi_studio_developer_task_centre_snapshot(
    UmiStudioDeveloperTaskCentre *centre,
    UmiStudioDeveloperTaskCentreSnapshot *out_snapshot)
{
    UmiProjectTaskRegistry *registry;
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry = task_registry(centre);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    copy_text(out_snapshot->area_id, sizeof(out_snapshot->area_id), "studio.developer-tasks");
    copy_text(out_snapshot->title, sizeof(out_snapshot->title), "Developer Tasks");
    copy_text(out_snapshot->summary, sizeof(out_snapshot->summary),
              "Project tasks submitted through the shared dependency-aware developer runtime.");
    out_snapshot->task_count = umi_project_task_registry_count(registry);
    out_snapshot->task_revision = umi_project_task_registry_revision(registry);
    out_snapshot->revision = centre->revision + out_snapshot->task_revision;
    out_snapshot->available = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_task_centre_at(
    UmiStudioDeveloperTaskCentre *centre,
    size_t index,
    UmiProjectTaskSnapshot *out_task)
{
    UmiProjectTaskRegistry *registry = task_registry(centre);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_project_task_registry_at(registry, index, out_task);
}

UmiStatus umi_studio_developer_task_centre_submit(
    UmiStudioDeveloperTaskCentre *centre,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_task_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiDeveloperRuntime *umi_studio_developer_task_centre_runtime(
    UmiStudioDeveloperTaskCentre *centre)
{
    return centre != NULL ? centre->runtime : NULL;
}

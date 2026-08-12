/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_run_centre.c
 *
 * PURPOSE:
 *   Implement Studio launch-profile browsing and run/debug submission.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_run_centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioDeveloperRunCentre {
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

static UmiProjectLaunchProfileRegistry *launch_registry(UmiStudioDeveloperRunCentre *centre)
{
    if (centre == NULL || centre->runtime == NULL) return NULL;
    return umi_project_workspace_launch_profile(
        umi_developer_runtime_projects(centre->runtime));
}

UmiStatus umi_studio_developer_run_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperRunCentre **out_centre)
{
    UmiStudioDeveloperRunCentre *centre;
    if (runtime == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioDeveloperRunCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->runtime = runtime;
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_developer_run_centre_destroy(UmiStudioDeveloperRunCentre *centre)
{
    free(centre);
}

UmiStatus umi_studio_developer_run_centre_snapshot(
    UmiStudioDeveloperRunCentre *centre,
    UmiStudioDeveloperRunCentreSnapshot *out_snapshot)
{
    UmiProjectLaunchProfileRegistry *registry;
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry = launch_registry(centre);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    copy_text(out_snapshot->area_id, sizeof(out_snapshot->area_id), "studio.developer-run");
    copy_text(out_snapshot->title, sizeof(out_snapshot->title), "Run and Debug");
    copy_text(out_snapshot->summary, sizeof(out_snapshot->summary),
              "Run and debug launch profiles executed through the common developer pipeline.");
    out_snapshot->launch_profile_count = umi_project_launch_profile_registry_count(registry);
    out_snapshot->launch_profile_revision = umi_project_launch_profile_registry_revision(registry);
    out_snapshot->revision = centre->revision + out_snapshot->launch_profile_revision;
    out_snapshot->available = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_run_centre_at(
    UmiStudioDeveloperRunCentre *centre,
    size_t index,
    UmiProjectLaunchProfileSnapshot *out_profile)
{
    UmiProjectLaunchProfileRegistry *registry = launch_registry(centre);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_project_launch_profile_registry_at(registry, index, out_profile);
}

UmiStatus umi_studio_developer_run_centre_submit(
    UmiStudioDeveloperRunCentre *centre,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    UmiStatus status;
    if (centre == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_developer_launch_plan_submit(centre->runtime, request, out_plan);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}

UmiDeveloperRuntime *umi_studio_developer_run_centre_runtime(
    UmiStudioDeveloperRunCentre *centre)
{
    return centre != NULL ? centre->runtime : NULL;
}

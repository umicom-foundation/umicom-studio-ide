/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_run_centre.h
 *
 * PURPOSE:
 *   Present project launch profiles and submit run/debug operations through the
 *   shared Framework developer runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEVELOPER_RUN_CENTRE_H
#define UMICOM_STUDIO_DEVELOPER_RUN_CENTRE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/developer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeveloperRunCentre UmiStudioDeveloperRunCentre;

typedef struct UmiStudioDeveloperRunCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char title[256];
    char summary[512];
    size_t launch_profile_count;
    uint64_t launch_profile_revision;
    uint64_t revision;
    int available;
} UmiStudioDeveloperRunCentreSnapshot;

UmiStatus umi_studio_developer_run_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperRunCentre **out_centre);
void umi_studio_developer_run_centre_destroy(UmiStudioDeveloperRunCentre *centre);
UmiStatus umi_studio_developer_run_centre_snapshot(
    UmiStudioDeveloperRunCentre *centre,
    UmiStudioDeveloperRunCentreSnapshot *out_snapshot);
UmiStatus umi_studio_developer_run_centre_at(
    UmiStudioDeveloperRunCentre *centre,
    size_t index,
    UmiProjectLaunchProfileSnapshot *out_profile);
UmiStatus umi_studio_developer_run_centre_submit(
    UmiStudioDeveloperRunCentre *centre,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan);
UmiDeveloperRuntime *umi_studio_developer_run_centre_runtime(
    UmiStudioDeveloperRunCentre *centre);

#ifdef __cplusplus
}
#endif
#endif

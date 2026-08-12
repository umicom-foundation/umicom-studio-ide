/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_task_centre.h
 *
 * PURPOSE:
 *   Present project tasks through the Studio developer workbench and submit
 *   selected tasks into the shared Framework developer pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The centre borrows the existing UmiDeveloperRuntime.  It does not create a
 * second project workspace, so tasks shown here are the same tasks visible to
 * the Project Centre and to future automation commands.
 */
#ifndef UMICOM_STUDIO_DEVELOPER_TASK_CENTRE_H
#define UMICOM_STUDIO_DEVELOPER_TASK_CENTRE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/developer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeveloperTaskCentre UmiStudioDeveloperTaskCentre;

typedef struct UmiStudioDeveloperTaskCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char title[256];
    char summary[512];
    size_t task_count;
    uint64_t task_revision;
    uint64_t revision;
    int available;
} UmiStudioDeveloperTaskCentreSnapshot;

UmiStatus umi_studio_developer_task_centre_create(
    UmiDeveloperRuntime *runtime,
    UmiStudioDeveloperTaskCentre **out_centre);
void umi_studio_developer_task_centre_destroy(UmiStudioDeveloperTaskCentre *centre);
UmiStatus umi_studio_developer_task_centre_snapshot(
    UmiStudioDeveloperTaskCentre *centre,
    UmiStudioDeveloperTaskCentreSnapshot *out_snapshot);
UmiStatus umi_studio_developer_task_centre_at(
    UmiStudioDeveloperTaskCentre *centre,
    size_t index,
    UmiProjectTaskSnapshot *out_task);
UmiStatus umi_studio_developer_task_centre_submit(
    UmiStudioDeveloperTaskCentre *centre,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan);
UmiDeveloperRuntime *umi_studio_developer_task_centre_runtime(
    UmiStudioDeveloperTaskCentre *centre);

#ifdef __cplusplus
}
#endif
#endif

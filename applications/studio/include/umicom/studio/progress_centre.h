/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/progress_centre.h
 *
 * PURPOSE:
 *   Compose the Progress Centre over Framework workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PROGRESS_CENTRE_H
#define UMICOM_STUDIO_PROGRESS_CENTRE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workbench_services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioProgressCentre UmiStudioProgressCentre;
typedef struct UmiStudioProgressCentreSnapshot {uint32_t struct_size;uint32_t api_version;char area_id[128];char title[256];char summary[512];size_t item_count;uint64_t revision;int available;} UmiStudioProgressCentreSnapshot;
UmiStatus umi_studio_progress_centre_create(UmiUiWorkbenchServices *services,UmiStudioProgressCentre **out_centre);
void umi_studio_progress_centre_destroy(UmiStudioProgressCentre *centre);
UmiStatus umi_studio_progress_centre_snapshot(UmiStudioProgressCentre *centre,UmiStudioProgressCentreSnapshot *out_snapshot);
UmiUiProgressRegistry *umi_studio_progress_centre_registry(UmiStudioProgressCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

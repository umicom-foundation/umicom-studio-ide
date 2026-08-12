/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/problems_centre.h
 *
 * PURPOSE:
 *   Compose the Problems Centre over Framework workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PROBLEMS_CENTRE_H
#define UMICOM_STUDIO_PROBLEMS_CENTRE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workbench_services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioProblemsCentre UmiStudioProblemsCentre;
typedef struct UmiStudioProblemsCentreSnapshot {uint32_t struct_size;uint32_t api_version;char area_id[128];char title[256];char summary[512];size_t item_count;uint64_t revision;int available;} UmiStudioProblemsCentreSnapshot;
UmiStatus umi_studio_problems_centre_create(UmiUiWorkbenchServices *services,UmiStudioProblemsCentre **out_centre);
void umi_studio_problems_centre_destroy(UmiStudioProblemsCentre *centre);
UmiStatus umi_studio_problems_centre_snapshot(UmiStudioProblemsCentre *centre,UmiStudioProblemsCentreSnapshot *out_snapshot);
UmiUiProblemRegistry *umi_studio_problems_centre_registry(UmiStudioProblemsCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/output_centre.h
 *
 * PURPOSE:
 *   Compose the Output Centre over Framework workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_OUTPUT_CENTRE_H
#define UMICOM_STUDIO_OUTPUT_CENTRE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workbench_services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioOutputCentre UmiStudioOutputCentre;
typedef struct UmiStudioOutputCentreSnapshot {uint32_t struct_size;uint32_t api_version;char area_id[128];char title[256];char summary[512];size_t item_count;uint64_t revision;int available;} UmiStudioOutputCentreSnapshot;
UmiStatus umi_studio_output_centre_create(UmiUiWorkbenchServices *services,UmiStudioOutputCentre **out_centre);
void umi_studio_output_centre_destroy(UmiStudioOutputCentre *centre);
UmiStatus umi_studio_output_centre_snapshot(UmiStudioOutputCentre *centre,UmiStudioOutputCentreSnapshot *out_snapshot);
UmiUiOutputChannelRegistry *umi_studio_output_centre_registry(UmiStudioOutputCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

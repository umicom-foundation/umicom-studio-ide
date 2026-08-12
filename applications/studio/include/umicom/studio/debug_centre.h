/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/debug_centre.h
 *
 * PURPOSE:
 *   Compose the Debug Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_DEBUG_CENTRE_H
#define UMICOM_STUDIO_DEBUG_CENTRE_H
#include <stdint.h>
#include "umicom/debug/service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDebugCentre UmiStudioDebugCentre;
typedef struct UmiStudioDebugCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiDebugServiceSnapshot service; uint64_t revision; int available;
} UmiStudioDebugCentreSnapshot;
UmiStatus umi_studio_debug_centre_create(UmiStudioDebugCentre **out_centre);
void umi_studio_debug_centre_destroy(UmiStudioDebugCentre *centre);
UmiStatus umi_studio_debug_centre_snapshot(UmiStudioDebugCentre *centre,UmiStudioDebugCentreSnapshot *out_snapshot);
UmiDebugService *umi_studio_debug_centre_service(UmiStudioDebugCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/source_control_centre.h
 *
 * PURPOSE:
 *   Compose the Source Control Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_SOURCE_CONTROL_CENTRE_H
#define UMICOM_STUDIO_SOURCE_CONTROL_CENTRE_H
#include <stdint.h>
#include "umicom/source_control/service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioSourceControlCentre UmiStudioSourceControlCentre;
typedef struct UmiStudioSourceControlCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiSourceControlServiceSnapshot service; uint64_t revision; int available;
} UmiStudioSourceControlCentreSnapshot;
UmiStatus umi_studio_source_control_centre_create(UmiStudioSourceControlCentre **out_centre);
void umi_studio_source_control_centre_destroy(UmiStudioSourceControlCentre *centre);
UmiStatus umi_studio_source_control_centre_snapshot(UmiStudioSourceControlCentre *centre,UmiStudioSourceControlCentreSnapshot *out_snapshot);
UmiSourceControlService *umi_studio_source_control_centre_service(UmiStudioSourceControlCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

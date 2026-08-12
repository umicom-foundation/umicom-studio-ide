/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/language_intelligence_centre.h
 *
 * PURPOSE:
 *   Compose the Language Intelligence Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_LANGUAGE_INTELLIGENCE_CENTRE_H
#define UMICOM_STUDIO_LANGUAGE_INTELLIGENCE_CENTRE_H
#include <stdint.h>
#include "umicom/language/service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioLanguageIntelligenceCentre UmiStudioLanguageIntelligenceCentre;
typedef struct UmiStudioLanguageIntelligenceCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiLanguageServiceSnapshot service; uint64_t revision; int available;
} UmiStudioLanguageIntelligenceCentreSnapshot;
UmiStatus umi_studio_language_intelligence_centre_create(UmiStudioLanguageIntelligenceCentre **out_centre);
void umi_studio_language_intelligence_centre_destroy(UmiStudioLanguageIntelligenceCentre *centre);
UmiStatus umi_studio_language_intelligence_centre_snapshot(UmiStudioLanguageIntelligenceCentre *centre,UmiStudioLanguageIntelligenceCentreSnapshot *out_snapshot);
UmiLanguageService *umi_studio_language_intelligence_centre_service(UmiStudioLanguageIntelligenceCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

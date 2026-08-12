/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/test_explorer_centre.h
 *
 * PURPOSE:
 *   Compose the Test Explorer Centre over reusable Umicom Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only. The reusable data model and service
 * remain in Umicom Framework so future applications can consume the same API.
 */
#ifndef UMICOM_STUDIO_TEST_EXPLORER_CENTRE_H
#define UMICOM_STUDIO_TEST_EXPLORER_CENTRE_H
#include <stdint.h>
#include "umicom/test_platform/service.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioTestExplorerCentre UmiStudioTestExplorerCentre;
typedef struct UmiStudioTestExplorerCentreSnapshot {
    uint32_t struct_size; uint32_t api_version;
    char area_id[128]; char title[256]; char summary[512];
    UmiTestPlatformServiceSnapshot service; uint64_t revision; int available;
} UmiStudioTestExplorerCentreSnapshot;
UmiStatus umi_studio_test_explorer_centre_create(UmiStudioTestExplorerCentre **out_centre);
void umi_studio_test_explorer_centre_destroy(UmiStudioTestExplorerCentre *centre);
UmiStatus umi_studio_test_explorer_centre_snapshot(UmiStudioTestExplorerCentre *centre,UmiStudioTestExplorerCentreSnapshot *out_snapshot);
UmiTestPlatformService *umi_studio_test_explorer_centre_service(UmiStudioTestExplorerCentre *centre);
#ifdef __cplusplus
}
#endif
#endif

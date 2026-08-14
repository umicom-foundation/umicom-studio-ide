/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/sdk_centre_v2.h
 *
 * PURPOSE:
 *   Present installed Framework SDK exports to Studio project consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SDK_CENTRE_V2_H
#define UMICOM_STUDIO_SDK_CENTRE_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSdkCentreV2 {
    UmiSdkExportPlanV2 export_plan;
    char consumer_cmake[2048];
    int ready;
} UmiStudioSdkCentreV2;

UmiStatus umi_studio_sdk_centre_v2_init(UmiStudioSdkCentreV2 *centre,
                                         const char *install_prefix);
UmiStatus umi_studio_sdk_centre_v2_refresh(UmiStudioSdkCentreV2 *centre);
const char *umi_studio_sdk_centre_v2_consumer_cmake(
    const UmiStudioSdkCentreV2 *centre);

#ifdef __cplusplus
}
#endif
#endif

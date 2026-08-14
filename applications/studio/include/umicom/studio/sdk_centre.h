/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/sdk_centre.h
 *
 * PURPOSE:
 *   Present installed Framework SDK exports to Studio project consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SDK_CENTRE_H
#define UMICOM_STUDIO_SDK_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSdkCentre {
    UmiSdkExportPlan export_plan;
    char consumer_cmake[2048];
    int ready;
} UmiStudioSdkCentre;

UmiStatus umi_studio_sdk_centre_init(UmiStudioSdkCentre *centre,
                                         const char *install_prefix);
UmiStatus umi_studio_sdk_centre_refresh(UmiStudioSdkCentre *centre);
const char *umi_studio_sdk_centre_consumer_cmake(
    const UmiStudioSdkCentre *centre);

#ifdef __cplusplus
}
#endif
#endif

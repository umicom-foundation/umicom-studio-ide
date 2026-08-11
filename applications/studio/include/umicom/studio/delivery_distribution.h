/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/delivery_distribution.h
 *
 * PURPOSE:
 *   Define Studio's supported distribution targets and runtime bundle defaults.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Distribution metadata tells the release system what Studio intends to ship on each supported platform.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_DISTRIBUTION_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_DISTRIBUTION_H

#include "umicom/umicom.h"
#include "umicom/studio/delivery_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_distribution_matrix(UmiPlatformMatrix *out_matrix);
UmiStatus umi_studio_runtime_bundle(UmiRuntimeBundle *out_bundle);

#ifdef __cplusplus
}
#endif

#endif

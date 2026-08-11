/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/conformance.h
 *
 * PURPOSE:
 *   Expose Studio release-candidate conformance checks over Framework ABI and product profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#ifndef UMICOM_STUDIO_CONFORMANCE_H
#define UMICOM_STUDIO_CONFORMANCE_H
#include "umicom/studio/compatibility_platform.h"
typedef struct UmiStudioConformanceResult { size_t products_checked; size_t products_compatible; int abi_ok; int passed; } UmiStudioConformanceResult;
UmiStatus umi_studio_conformance_run(UmiStudioCompatibilityPlatform *platform,UmiStudioConformanceResult *out_result);
#endif


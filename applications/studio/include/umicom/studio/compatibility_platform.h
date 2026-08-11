/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/compatibility_platform.h
 *
 * PURPOSE:
 *   Own Studio access to Framework ABI, SDK and product-conformance services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#ifndef UMICOM_STUDIO_COMPATIBILITY_PLATFORM_H
#define UMICOM_STUDIO_COMPATIBILITY_PLATFORM_H
#include "umicom/umicom.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioCompatibilityPlatform UmiStudioCompatibilityPlatform;
UmiStatus umi_studio_compatibility_platform_create(UmiStudioCompatibilityPlatform **out_platform);
void umi_studio_compatibility_platform_destroy(UmiStudioCompatibilityPlatform *platform);
size_t umi_studio_compatibility_product_count(const UmiStudioCompatibilityPlatform *platform);
const UmiProductProfile *umi_studio_compatibility_product_at(const UmiStudioCompatibilityPlatform *platform,size_t index);
UmiStatus umi_studio_compatibility_summary(const UmiStudioCompatibilityPlatform *platform,char *buffer,size_t capacity);
#endif


/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/delivery_platform.h
 *
 * PURPOSE:
 *   Own Studio's shared Framework delivery, packaging, release and update composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes Framework delivery services here; packaging mechanics remain reusable inside Framework.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_PLATFORM_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_PLATFORM_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeliveryPlatform UmiStudioDeliveryPlatform;

UmiStatus umi_studio_delivery_platform_create(UmiStudioDeliveryPlatform **out_platform);
void umi_studio_delivery_platform_destroy(UmiStudioDeliveryPlatform *platform);

UmiReleaseRegistry *umi_studio_delivery_releases(UmiStudioDeliveryPlatform *platform);
UmiGenerationStore *umi_studio_delivery_generations(UmiStudioDeliveryPlatform *platform);
UmiDeliveryPipeline *umi_studio_delivery_pipeline(UmiStudioDeliveryPlatform *platform);
UmiChannelRegistry *umi_studio_delivery_channels(UmiStudioDeliveryPlatform *platform);
const UmiDeliveryPolicy *umi_studio_delivery_policy(const UmiStudioDeliveryPlatform *platform);

#ifdef __cplusplus
}
#endif

#endif

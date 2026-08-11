/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/delivery_platform.c
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

#include "umicom/studio/delivery_platform.h"
#include <stdlib.h>

struct UmiStudioDeliveryPlatform {
    UmiReleaseRegistry releases;
    UmiGenerationStore generations;
    UmiDeliveryPipeline pipeline;
    UmiChannelRegistry channels;
    UmiDeliveryPolicy policy;
};

UmiStatus umi_studio_delivery_platform_create(UmiStudioDeliveryPlatform **out_platform)
{
    UmiStudioDeliveryPlatform *platform;
    UmiReleaseGate gate;
    if (out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;
    platform = (UmiStudioDeliveryPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_release_registry_init(&platform->releases);
    umi_generation_store_init(&platform->generations);
    umi_channel_registry_init(&platform->channels);
    platform->policy = umi_delivery_policy_default();
    (void)umi_channel_registry_add(&platform->channels,
                                   "development",
                                   umi_release_channel_policy(UMI_RELEASE_DEVELOPMENT));
    (void)umi_channel_registry_add(&platform->channels,
                                   "beta",
                                   umi_release_channel_policy(UMI_RELEASE_BETA));
    (void)umi_channel_registry_add(&platform->channels,
                                   "stable",
                                   umi_release_channel_policy(UMI_RELEASE_STABLE));
    if (umi_delivery_pipeline_init(&platform->pipeline, "studio.release") != UMI_STATUS_OK) {
        free(platform);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    (void)umi_release_gate_init(&gate, "build", 1);
    (void)umi_delivery_pipeline_add_gate(&platform->pipeline, &gate);
    (void)umi_release_gate_init(&gate, "tests", 1);
    (void)umi_delivery_pipeline_add_gate(&platform->pipeline, &gate);
    (void)umi_release_gate_init(&gate, "package", 1);
    (void)umi_delivery_pipeline_add_gate(&platform->pipeline, &gate);
    (void)umi_release_gate_init(&gate, "verification", 1);
    (void)umi_delivery_pipeline_add_gate(&platform->pipeline, &gate);

    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_studio_delivery_platform_destroy(UmiStudioDeliveryPlatform *platform)
{
    free(platform);
}

UmiReleaseRegistry *umi_studio_delivery_releases(UmiStudioDeliveryPlatform *platform)
{
    return platform != NULL ? &platform->releases : NULL;
}
UmiGenerationStore *umi_studio_delivery_generations(UmiStudioDeliveryPlatform *platform)
{
    return platform != NULL ? &platform->generations : NULL;
}
UmiDeliveryPipeline *umi_studio_delivery_pipeline(UmiStudioDeliveryPlatform *platform)
{
    return platform != NULL ? &platform->pipeline : NULL;
}
UmiChannelRegistry *umi_studio_delivery_channels(UmiStudioDeliveryPlatform *platform)
{
    return platform != NULL ? &platform->channels : NULL;
}
const UmiDeliveryPolicy *umi_studio_delivery_policy(const UmiStudioDeliveryPlatform *platform)
{
    return platform != NULL ? &platform->policy : NULL;
}

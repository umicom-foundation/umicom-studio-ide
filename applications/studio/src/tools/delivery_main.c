/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/delivery_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for the Framework-backed application-delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command is a thin product-facing wrapper; the reusable delivery logic lives in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/studio/services.h"
#include "umicom/studio/delivery_platform.h"
int main(void) {
    UmiStudioServices *services = NULL;
    UmiStudioDeliveryPlatform *platform;
    UmiDeliveryPipeline *pipeline;
    if (umi_studio_services_create(NULL, NULL, &services) != UMI_STATUS_OK) return 1;
    platform = umi_studio_services_delivery_platform(services);
    pipeline = umi_studio_delivery_pipeline(platform);
    printf("Delivery pipeline: %s\n", pipeline->pipeline_id);
    printf("Release gates: %zu\n", pipeline->gate_count);
    printf("Channels: %zu\n", umi_studio_delivery_channels(platform)->count);
    umi_studio_services_destroy(services);
    return 0;
}

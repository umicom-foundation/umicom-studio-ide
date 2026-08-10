/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/replay_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for deterministic replay of durable Integration Fabric records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/messages.h"
#include "umicom/studio/replay.h"

#include <stdio.h>

static UmiStatus print_replayed(const UmiMessageEnvelope *message, void *user_data)
{
    size_t *count = (size_t *)user_data;
    (*count)++;
    (void)printf("Replay %llu: %s\n",
                 (unsigned long long)message->sequence,
                 message->name);
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiSubscription subscription = umi_subscription_all();
    UmiStudioReplayRequest request = umi_studio_replay_request_default();
    UmiStatus status;
    uint64_t subscription_id = 0U;
    size_t observer_count = 0U;
    size_t replayed = 0U;
    status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) return 1;
    services = umi_studio_bootstrap_services(bootstrap);
    status = umi_studio_messages_subscribe(services,
                                           &subscription,
                                           print_replayed,
                                           &observer_count,
                                           &subscription_id);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_replay(services, &request, &replayed);
    }
    (void)umi_studio_messages_unsubscribe(services, subscription_id);
    if (status == UMI_STATUS_OK) {
        (void)printf("Replayed: %zu; observed: %zu\n", replayed, observer_count);
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}

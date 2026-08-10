/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_replay.c
 *
 * PURPOSE:
 *   Verify deterministic Studio journal replay through the shared dispatcher.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/messages.h"
#include "umicom/studio/replay.h"

#include <assert.h>

static UmiStatus receive(const UmiMessageEnvelope *message, void *user_data)
{
    size_t *count = (size_t *)user_data;
    assert((message->flags & UMI_MESSAGE_FLAG_REPLAY) != 0U ||
           (message->flags & UMI_MESSAGE_FLAG_DURABLE) != 0U);
    (*count)++;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioPublishRequest publish = umi_studio_publish_request_default();
    UmiStudioReplayRequest replay = umi_studio_replay_request_default();
    UmiSubscription subscription = umi_subscription_all();
    uint64_t id = 0U;
    size_t count = 0U;
    size_t replayed = 0U;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    publish.name = "studio.replay.test";
    publish.payload = "one";
    assert(umi_studio_messages_publish(services, &publish, NULL, NULL) == UMI_STATUS_OK);
    assert(umi_studio_messages_subscribe(services, &subscription, receive,
                                         &count, &id) == UMI_STATUS_OK);
    assert(umi_studio_replay(services, &replay, &replayed) == UMI_STATUS_OK);
    assert(replayed == 1U && count == 1U);
    assert(umi_studio_messages_unsubscribe(services, id) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

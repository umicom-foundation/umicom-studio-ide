/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/replay.c
 *
 * PURPOSE:
 *   Implement filtered deterministic Studio replay by adapting the durable journal to the Framework replay contract and dispatching replay-marked messages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/replay.h"

#include <string.h>

static UmiStatus replay_dispatch(const UmiMessageEnvelope *message,
                                 void *user_data)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiMessageEnvelope replayed = *message;
    size_t deliveries = 0U;
    UmiStatus status;
    replayed.flags |= UMI_MESSAGE_FLAG_REPLAY;
    status = umi_dispatcher_dispatch(umi_studio_services_dispatcher(services),
                                     &replayed,
                                     &deliveries);
    if (status == UMI_STATUS_OK) {
        umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                      UMI_MESSAGE_METRIC_REPLAYED);
    }
    return status;
}

UmiStudioReplayRequest umi_studio_replay_request_default(void)
{
    UmiStudioReplayRequest request;
    (void)memset(&request, 0, sizeof(request));
    request.maximum_sequence = UINT64_MAX;
    request.maximum_messages = SIZE_MAX;
    return request;
}

UmiStatus umi_studio_replay(UmiStudioServices *services,
                            const UmiStudioReplayRequest *request,
                            size_t *out_replayed)
{
    UmiReplayOptions options;
    UmiReplaySource source;
    if (services == NULL || request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    options = umi_replay_options_default();
    options.minimum_sequence = request->minimum_sequence;
    options.maximum_sequence = request->maximum_sequence;
    options.correlation_id = request->correlation_id;
    options.maximum_messages = request->maximum_messages;
    source = umi_message_store_replay_source(
        umi_studio_services_journal(services)->messages);
    return umi_replay_execute(&source,
                              &options,
                              replay_dispatch,
                              services,
                              out_replayed);
}

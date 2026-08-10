/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/replay.h
 *
 * PURPOSE:
 *   Declare deterministic replay of Studio durable journal messages through the shared Framework dispatcher with sequence and correlation filters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_REPLAY_H
#define UMICOM_STUDIO_REPLAY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioReplayRequest {
    uint64_t minimum_sequence;
    uint64_t maximum_sequence;
    uint64_t correlation_id;
    size_t maximum_messages;
} UmiStudioReplayRequest;

UmiStudioReplayRequest umi_studio_replay_request_default(void);
UmiStatus umi_studio_replay(UmiStudioServices *services,
                            const UmiStudioReplayRequest *request,
                            size_t *out_replayed);

#ifdef __cplusplus
}
#endif

#endif

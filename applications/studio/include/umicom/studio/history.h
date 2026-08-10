/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/history.h
 *
 * PURPOSE:
 *   Declare persistent Studio operational-history records stored through the Framework message store for build, diagnostic, task and application events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_HISTORY_H
#define UMICOM_STUDIO_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioHistoryRecord {
    uint64_t sequence;
    uint64_t correlation_id;
    char category[64];
    UmiStatus status;
    char message[512];
} UmiStudioHistoryRecord;

UmiStatus umi_studio_history_append(UmiStudioServices *services,
                                    const char *category,
                                    UmiStatus status,
                                    const char *message,
                                    uint64_t correlation_id,
                                    uint64_t *out_sequence);
size_t umi_studio_history_count(UmiStudioServices *services);
UmiStatus umi_studio_history_read(UmiStudioServices *services,
                                  size_t index,
                                  UmiStudioHistoryRecord *out_record);

#ifdef __cplusplus
}
#endif

#endif

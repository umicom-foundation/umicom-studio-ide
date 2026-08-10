/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/history.c
 *
 * PURPOSE:
 *   Implement Studio operational-history serialisation and ordered reads over the Framework durable message store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/messages.h"

UmiStatus umi_studio_history_append(UmiStudioServices *services,
                                    const char *category,
                                    UmiStatus status,
                                    const char *message,
                                    uint64_t correlation_id,
                                    uint64_t *out_sequence)
{
    UmiMessageEnvelope envelope;
    char payload[768];
    int written;
    if (services == NULL || category == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(payload,
                       sizeof(payload),
                       "%s\n%d\n%s",
                       category,
                       (int)status,
                       message);
    if (written < 0 || (size_t)written >= sizeof(payload)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_message_envelope_init(&envelope,
                              UMI_MESSAGE_EVENT,
                              "studio.history.recorded",
                              payload);
    envelope.schema_id = UMI_STUDIO_SCHEMA_HISTORY;
    envelope.flags = UMI_MESSAGE_FLAG_DURABLE;
    envelope.correlation_id = correlation_id != 0U
        ? correlation_id : envelope.message_id;
    envelope.source = "org.umicom.studio.history";
    envelope.created_at_nanoseconds =
        umi_studio_services_clock(services)->wall_nanoseconds(
            umi_studio_services_clock(services));
    return umi_message_store_append(umi_studio_services_message_store(services),
                                    &envelope,
                                    out_sequence);
}

size_t umi_studio_history_count(UmiStudioServices *services)
{
    return services != NULL
        ? umi_message_store_count(umi_studio_services_message_store(services))
        : 0U;
}

UmiStatus umi_studio_history_read(UmiStudioServices *services,
                                  size_t index,
                                  UmiStudioHistoryRecord *out_record)
{
    UmiOwnedMessage message;
    char *text;
    char *line2;
    char *line3;
    char *end = NULL;
    long parsed;
    UmiStatus status;
    if (services == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_message_store_read(umi_studio_services_message_store(services),
                                    index,
                                    &message);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_record, 0, sizeof(*out_record));
    text = message.payload_storage;
    line2 = strchr(text, '\n');
    if (line2 == NULL) {
        umi_message_dispose(&message);
        return UMI_STATUS_PARSE_ERROR;
    }
    *line2++ = '\0';
    line3 = strchr(line2, '\n');
    if (line3 == NULL) {
        umi_message_dispose(&message);
        return UMI_STATUS_PARSE_ERROR;
    }
    *line3++ = '\0';
    parsed = strtol(line2, &end, 10);
    if (end == line2 || *end != '\0') {
        umi_message_dispose(&message);
        return UMI_STATUS_PARSE_ERROR;
    }
    out_record->sequence = message.envelope.sequence;
    out_record->correlation_id = message.envelope.correlation_id;
    out_record->status = (UmiStatus)parsed;
    (void)snprintf(out_record->category,
                   sizeof(out_record->category),
                   "%s",
                   text);
    (void)snprintf(out_record->message,
                   sizeof(out_record->message),
                   "%s",
                   line3);
    umi_message_dispose(&message);
    return UMI_STATUS_OK;
}

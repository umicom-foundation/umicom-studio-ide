/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/messages.c
 *
 * PURPOSE:
 *   Implement Studio typed publication, schema-aware dispatch, journal persistence, transactional outbox delivery, dead-letter retention and metrics through Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/messages.h"

#include <string.h>

UmiStudioPublishRequest umi_studio_publish_request_default(void)
{
    UmiStudioPublishRequest request;
    (void)memset(&request, 0, sizeof(request));
    request.kind = UMI_MESSAGE_EVENT;
    request.schema_id = UMI_STUDIO_SCHEMA_EVENT;
    request.flags = UMI_MESSAGE_FLAG_DURABLE;
    return request;
}

UmiStatus umi_studio_messages_subscribe(UmiStudioServices *services,
                                        const UmiSubscription *subscription,
                                        UmiMessageHandler handler,
                                        void *user_data,
                                        uint64_t *out_subscription_id)
{
    UmiDispatcher *dispatcher = umi_studio_services_dispatcher(services);
    return dispatcher != NULL
        ? umi_dispatcher_subscribe(dispatcher,
                                   subscription,
                                   handler,
                                   user_data,
                                   out_subscription_id)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_messages_unsubscribe(UmiStudioServices *services,
                                          uint64_t subscription_id)
{
    UmiDispatcher *dispatcher = umi_studio_services_dispatcher(services);
    return dispatcher != NULL
        ? umi_dispatcher_unsubscribe(dispatcher, subscription_id)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_messages_publish(UmiStudioServices *services,
                                      const UmiStudioPublishRequest *request,
                                      uint64_t *out_sequence,
                                      size_t *out_deliveries)
{
    UmiMessageEnvelope message;
    UmiStatus status;
    uint64_t outbox_record = 0U;
    uint64_t sequence = 0U;
    size_t deliveries = 0U;
    if (services == NULL || request == NULL || request->name == NULL ||
        request->name[0] == '\0' || request->schema_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_message_envelope_init(&message,
                              request->kind,
                              request->name,
                              request->payload != NULL ? request->payload : "");
    message.schema_id = request->schema_id;
    message.schema_version = 1U;
    message.source = "org.umicom.studio";
    message.destination = request->destination;
    message.partition_key = request->partition_key;
    message.flags = request->flags;
    message.correlation_id = request->correlation_id != 0U
        ? request->correlation_id : message.message_id;
    message.causation_id = request->causation_id;
    message.created_at_nanoseconds =
        umi_studio_services_clock(services)->wall_nanoseconds(
            umi_studio_services_clock(services));

    status = umi_schema_registry_validate(
        umi_studio_services_schema_registry(services), &message);
    if (status != UMI_STATUS_OK) {
        umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                      UMI_MESSAGE_METRIC_FAILED);
        return status;
    }
    umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                  UMI_MESSAGE_METRIC_ACCEPTED);

    if ((message.flags & UMI_MESSAGE_FLAG_DURABLE) != 0U) {
        status = umi_journal_store_append(umi_studio_services_journal(services),
                                          &message,
                                          &sequence);
        if (status != UMI_STATUS_OK) return status;
        message.sequence = sequence;
        status = umi_outbox_enqueue(umi_studio_services_outbox(services),
                                    &message,
                                    &outbox_record);
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_dispatcher_dispatch(umi_studio_services_dispatcher(services),
                                     &message,
                                     &deliveries);
    if (outbox_record != 0U) {
        (void)umi_outbox_complete(umi_studio_services_outbox(services),
                                  outbox_record,
                                  status,
                                  umi_retry_status_is_retryable(status));
    }
    if (status == UMI_STATUS_OK) {
        umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                      UMI_MESSAGE_METRIC_DELIVERED);
    } else {
        umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                      UMI_MESSAGE_METRIC_FAILED);
    }
    if (out_sequence != NULL) *out_sequence = sequence;
    if (out_deliveries != NULL) *out_deliveries = deliveries;
    return status;
}

UmiStatus umi_studio_messages_flush_outbox(UmiStudioServices *services,
                                           size_t maximum_records,
                                           size_t *out_delivered,
                                           size_t *out_failed)
{
    size_t delivered = 0U;
    size_t failed = 0U;
    size_t processed = 0U;
    UmiOutboxRecordView record;
    UmiStatus status;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (maximum_records == 0U || processed < maximum_records) {
        size_t deliveries = 0U;
        status = umi_outbox_next_pending(umi_studio_services_outbox(services),
                                         &record);
        if (status == UMI_STATUS_NOT_FOUND) break;
        if (status != UMI_STATUS_OK) return status;
        status = umi_dispatcher_dispatch(umi_studio_services_dispatcher(services),
                                         record.message,
                                         &deliveries);
        if (status == UMI_STATUS_OK) {
            delivered++;
            umi_message_metrics_increment(umi_studio_services_message_metrics(services),
                                          UMI_MESSAGE_METRIC_DELIVERED);
        } else {
            int retryable = umi_retry_status_is_retryable(status);
            failed++;
            if (!retryable) {
                (void)umi_dead_letter_store_add(
                    umi_studio_services_dead_letters(services),
                    record.message,
                    status,
                    umi_status_text(status),
                    record.attempts,
                    umi_studio_services_clock(services)->wall_nanoseconds(
                        umi_studio_services_clock(services)),
                    NULL);
                umi_message_metrics_increment(
                    umi_studio_services_message_metrics(services),
                    UMI_MESSAGE_METRIC_DEAD_LETTERED);
            }
            (void)umi_outbox_complete(umi_studio_services_outbox(services),
                                      record.record_id,
                                      status,
                                      retryable);
        }
        if (status == UMI_STATUS_OK) {
            (void)umi_outbox_complete(umi_studio_services_outbox(services),
                                      record.record_id,
                                      UMI_STATUS_OK,
                                      0);
        }
        processed++;
    }
    (void)umi_outbox_remove_sent(umi_studio_services_outbox(services), NULL);
    if (out_delivered != NULL) *out_delivered = delivered;
    if (out_failed != NULL) *out_failed = failed;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_messages_report(UmiStudioServices *services,
                                     UmiStudioMessageReport *out_report)
{
    if (services == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->schemas = umi_schema_registry_count(
        umi_studio_services_schema_registry(services));
    out_report->topics = umi_topic_registry_count(
        umi_studio_services_topics(services));
    out_report->durable_messages = umi_message_store_count(
        umi_studio_services_message_store(services));
    out_report->journal_messages = umi_journal_store_count(
        umi_studio_services_journal(services));
    out_report->dispatcher = umi_dispatcher_stats(
        umi_studio_services_dispatcher(services));
    out_report->inbox = umi_inbox_stats(umi_studio_services_inbox(services));
    out_report->outbox = umi_outbox_stats(umi_studio_services_outbox(services));
    out_report->dead_letters = umi_dead_letter_store_count(
        umi_studio_services_dead_letters(services));
    out_report->metrics = umi_message_metrics_snapshot(
        umi_studio_services_message_metrics(services));
    return UMI_STATUS_OK;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_messages.c
 *
 * PURPOSE:
 *   Verify Studio schema-aware publication, subscription delivery, journal persistence and Integration Fabric reports.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/messages.h"

#include <assert.h>

static UmiStatus receive(const UmiMessageEnvelope *message, void *user_data)
{
    size_t *count = (size_t *)user_data;
    assert(message->schema_id != NULL);
    (*count)++;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiSubscription subscription = umi_subscription_all();
    UmiStudioPublishRequest request = umi_studio_publish_request_default();
    UmiStudioMessageReport report;
    uint64_t subscription_id = 0U;
    uint64_t sequence = 0U;
    size_t received = 0U;
    size_t deliveries = 0U;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_messages_subscribe(services, &subscription, receive,
                                         &received, &subscription_id) == UMI_STATUS_OK);
    request.name = "studio.test.event";
    request.payload = "payload";
    assert(umi_studio_messages_publish(services, &request,
                                       &sequence, &deliveries) == UMI_STATUS_OK);
    assert(sequence == 1U);
    assert(deliveries == 1U && received == 1U);
    assert(umi_studio_messages_report(services, &report) == UMI_STATUS_OK);
    assert(report.schemas == 4U);
    assert(report.journal_messages == 1U);
    assert(umi_studio_messages_unsubscribe(services, subscription_id) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_outbox.c
 *
 * PURPOSE:
 *   Verify Studio transactional outbox publication and sent-record compaction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/messages.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioPublishRequest request = umi_studio_publish_request_default();
    UmiStudioMessageReport report;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    request.name = "studio.outbox.test";
    request.payload = "outbox";
    assert(umi_studio_messages_publish(services, &request, NULL, NULL) == UMI_STATUS_OK);
    assert(umi_studio_messages_report(services, &report) == UMI_STATUS_OK);
    assert(report.outbox.sent == 1U);
    assert(umi_studio_messages_flush_outbox(services, 0U, NULL, NULL) == UMI_STATUS_OK);
    assert(umi_studio_messages_report(services, &report) == UMI_STATUS_OK);
    assert(report.outbox.records == 0U);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

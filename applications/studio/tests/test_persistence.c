/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_persistence.c
 *
 * PURPOSE:
 *   Verify Framework-backed Studio store and durable journal services share one authoritative Data Server.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/data.h"
#include "umicom/studio/messages.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioPublishRequest request = umi_studio_publish_request_default();
    UmiStudioDataReport data;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_data_set(services, "persistence.key", "value") == UMI_STATUS_OK);
    request.name = "studio.persistence.test";
    request.payload = "persistent";
    assert(umi_studio_messages_publish(services, &request, NULL, NULL) == UMI_STATUS_OK);
    assert(umi_studio_data_report(services, &data) == UMI_STATUS_OK);
    assert(data.records >= 3U);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_dead_letters.c
 *
 * PURPOSE:
 *   Verify Studio dead-letter retention through the Framework message-failure model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiMessageEnvelope message;
    UmiDeadLetterView view;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT,
                              "studio.failed", "payload");
    assert(umi_dead_letter_store_add(umi_studio_services_dead_letters(services),
                                     &message,
                                     UMI_STATUS_INTERNAL_ERROR,
                                     "test failure",
                                     1U,
                                     1U,
                                     NULL) == UMI_STATUS_OK);
    assert(umi_dead_letter_store_get(umi_studio_services_dead_letters(services),
                                     0U, &view) == UMI_STATUS_OK);
    assert(view.status == UMI_STATUS_INTERNAL_ERROR);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}

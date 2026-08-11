/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_services.c
 *
 * PURPOSE:
 *   Verify one Studio integration path over the Framework delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test keeps product integration small so failures are easy for a new developer to locate.
 */

#include <assert.h>
#include "umicom/studio/services.h"
int main(void) {
    UmiStudioServices *services = NULL;
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_services_delivery_platform(services) != NULL);
    umi_studio_services_destroy(services);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_platform.c
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
#include "umicom/studio/delivery_platform.h"
int main(void) {
    UmiStudioDeliveryPlatform *platform = NULL;
    assert(umi_studio_delivery_platform_create(&platform) == UMI_STATUS_OK);
    assert(umi_studio_delivery_pipeline(platform)->gate_count == 4U);
    assert(umi_studio_delivery_channels(platform)->count == 3U);
    umi_studio_delivery_platform_destroy(platform);
    return 0;
}

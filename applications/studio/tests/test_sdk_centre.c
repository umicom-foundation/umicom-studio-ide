/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_sdk_centre.c
 *
 * PURPOSE:
 *   Verify Studio's installed Framework SDK consumer view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/sdk_centre.h"

int main(void)
{
    UmiStudioSdkCentre centre;
    const char *cmake;
    assert(umi_studio_sdk_centre_init(
               &centre, "C:/Dev/umicom/install/framework") == UMI_STATUS_OK);
    assert(centre.ready);
    assert(centre.export_plan.catalogue.count == 8U);
    cmake = umi_studio_sdk_centre_consumer_cmake(&centre);
    assert(cmake != NULL);
    assert(strstr(cmake, "Umicom::compiler") != NULL);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_package.c
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
#include "umicom/studio/delivery_package.h"
int main(void) {
    UmiPackageSpec spec;
    UmiPackageResult result;
    assert(umi_studio_package_spec("stage", "dist", &spec) == UMI_STATUS_OK);
    assert(umi_studio_package_directory(&spec, &result) == UMI_STATUS_OK);
    assert(result.succeeded);
    return 0;
}

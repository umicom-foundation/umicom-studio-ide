/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_distribution.c
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
#include "umicom/studio/delivery_distribution.h"
int main(void) {
    UmiPlatformMatrix matrix;
    UmiRuntimeBundle bundle;
    assert(umi_studio_distribution_matrix(&matrix) == UMI_STATUS_OK);
    assert(umi_platform_matrix_supports(&matrix, "windows", "x86_64"));
    assert(umi_studio_runtime_bundle(&bundle) == UMI_STATUS_OK);
    assert(bundle.count >= 2U);
    return 0;
}

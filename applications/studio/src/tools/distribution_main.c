/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/distribution_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for the Framework-backed application-delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command is a thin product-facing wrapper; the reusable delivery logic lives in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/studio/delivery_distribution.h"
int main(void) {
    UmiPlatformMatrix matrix;
    UmiRuntimeBundle bundle;
    if (umi_studio_distribution_matrix(&matrix) != UMI_STATUS_OK) return 1;
    if (umi_studio_runtime_bundle(&bundle) != UMI_STATUS_OK) return 2;
    printf("Distribution targets: %zu\n", matrix.count);
    printf("Runtime bundle files: %zu\n", bundle.count);
    return 0;
}

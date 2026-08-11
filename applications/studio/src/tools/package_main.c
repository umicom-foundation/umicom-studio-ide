/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/package_main.c
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
#include "umicom/studio/delivery_package.h"
int main(void) {
    UmiPackageSpec spec;
    UmiPackageResult result;
    if (umi_studio_package_spec("stage", "dist/umicom-studio", &spec) != UMI_STATUS_OK) return 1;
    if (umi_studio_package_directory(&spec, &result) != UMI_STATUS_OK) return 2;
    printf("Package: %s\n", result.output_path);
    printf("Format: %s\n", umi_package_format_text(spec.format));
    return result.succeeded ? 0 : 3;
}

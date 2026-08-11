/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/evidence_main.c
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
#include "umicom/studio/delivery_evidence.h"
int main(void) {
    UmiBuildEvidence evidence;
    UmiSbom sbom;
    UmiLicenseInventory licenses;
    if (umi_studio_delivery_build_evidence("local", 10U, 0U, 0U, &evidence) != UMI_STATUS_OK) return 1;
    if (umi_studio_delivery_sbom(&sbom, &licenses) != UMI_STATUS_OK) return 2;
    printf("Build evidence passed: %s\n", umi_build_evidence_passed(&evidence) ? "yes" : "no");
    printf("SBOM components: %zu\n", sbom.count);
    printf("Licence entries: %zu\n", licenses.count);
    return 0;
}

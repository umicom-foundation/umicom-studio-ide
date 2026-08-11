/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_evidence.c
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
#include "umicom/studio/delivery_evidence.h"
int main(void) {
    UmiBuildEvidence evidence;
    UmiSbom sbom;
    UmiLicenseInventory licenses;
    assert(umi_studio_delivery_build_evidence("sha", 3U, 0U, 0U, &evidence) == UMI_STATUS_OK);
    assert(umi_build_evidence_passed(&evidence));
    assert(umi_studio_delivery_sbom(&sbom, &licenses) == UMI_STATUS_OK);
    assert(sbom.count == 2U);
    return 0;
}

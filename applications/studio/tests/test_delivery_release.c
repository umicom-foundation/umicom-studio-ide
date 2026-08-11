/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_release.c
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
#include "umicom/studio/delivery_release.h"
#include "umicom/studio/delivery_evidence.h"
int main(void) {
    UmiStudioDeliveryPlatform *platform = NULL;
    UmiReleaseCandidate candidate;
    UmiDeliveryArtifact artifact;
    assert(umi_studio_delivery_platform_create(&platform) == UMI_STATUS_OK);
    assert(umi_studio_release_prepare(platform, "r", "sha", UMI_RELEASE_DEVELOPMENT, &candidate) == UMI_STATUS_OK);
    assert(umi_studio_delivery_build_evidence("sha", 1U, 0U, 0U, &candidate.build) == UMI_STATUS_OK);
    assert(umi_delivery_artifact_init(&artifact, "app", UMI_ARTIFACT_EXECUTABLE, "bin/app") == UMI_STATUS_OK);
    assert(umi_artifact_set_add(&candidate.artifacts, &artifact) == UMI_STATUS_OK);
    assert(umi_verification_report_add(&candidate.verification, "integrity", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_studio_release_register(platform, &candidate, 1U) == UMI_STATUS_OK);
    umi_studio_delivery_platform_destroy(platform);
    return 0;
}

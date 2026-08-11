/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/release_main.c
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
#include "umicom/studio/delivery_platform.h"
#include "umicom/studio/delivery_release.h"
#include "umicom/studio/delivery_evidence.h"
int main(void) {
    UmiStudioDeliveryPlatform *platform = NULL;
    UmiReleaseCandidate candidate;
    UmiDeliveryArtifact artifact;
    if (umi_studio_delivery_platform_create(&platform) != UMI_STATUS_OK) return 1;
    if (umi_studio_release_prepare(platform, "studio-local", "working-tree", UMI_RELEASE_DEVELOPMENT, &candidate) != UMI_STATUS_OK) return 2;
    if (umi_studio_delivery_build_evidence("working-tree", 1U, 0U, 0U, &candidate.build) != UMI_STATUS_OK) return 3;
    if (umi_delivery_artifact_init(&artifact, "studio", UMI_ARTIFACT_EXECUTABLE, "bin/umicom-studio-ide") != UMI_STATUS_OK) return 4;
    (void)umi_artifact_set_add(&candidate.artifacts, &artifact);
    (void)umi_verification_report_add(&candidate.verification, "local", UMI_EVIDENCE_PASS);
    printf("Release candidate ready: %s\n", umi_release_candidate_ready(&candidate) ? "yes" : "no");
    umi_studio_delivery_platform_destroy(platform);
    return 0;
}

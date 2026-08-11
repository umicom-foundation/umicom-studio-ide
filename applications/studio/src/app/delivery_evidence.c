/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/delivery_evidence.c
 *
 * PURPOSE:
 *   Populate Studio build, SBOM and licence evidence used by release gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Evidence helpers provide deterministic test data now and a stable place to connect real build results later.
 */

#include "umicom/studio/delivery_evidence.h"
#include <stdio.h>
#include <string.h>

UmiStatus umi_studio_delivery_build_evidence(const char *source_revision,
                                             uint32_t tests_passed,
                                             uint32_t tests_failed,
                                             uint32_t warnings,
                                             UmiBuildEvidence *out_evidence)
{
    if (source_revision == NULL || out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_build_evidence_init(out_evidence);
    out_evidence->build_succeeded = 1;
    out_evidence->warnings = warnings;
    out_evidence->tests_total = tests_passed + tests_failed;
    out_evidence->tests_passed = tests_passed;
    out_evidence->tests_failed = tests_failed;
    if (umi_build_evidence_set_source(out_evidence, source_revision) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(out_evidence->compiler, sizeof(out_evidence->compiler), "%s", "configured");
    (void)snprintf(out_evidence->preset, sizeof(out_evidence->preset), "%s", "studio");
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_delivery_sbom(UmiSbom *out_sbom,
                                   UmiLicenseInventory *out_licenses)
{
    if (out_sbom == NULL || out_licenses == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_sbom_init(out_sbom);
    umi_license_inventory_init(out_licenses);
    if (umi_sbom_add(out_sbom, "Umicom Framework", "0.5.0",
                     "Umicom Foundation", "MIT") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_sbom_add(out_sbom, "Umicom Studio IDE", "0.14.0",
                     "Umicom Foundation", "MIT") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_license_inventory_add(out_licenses, "Umicom Framework", "MIT", "LICENSE") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_license_inventory_add(out_licenses, "Umicom Studio IDE", "MIT", "LICENSE");
}

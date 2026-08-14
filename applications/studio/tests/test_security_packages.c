/* Umicom Studio IDE Tests | Security package trust v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "security_centre_fixture.h"
#include "umicom/studio/security_packages.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_seeded_security_centre();
    const UmiSecurityPackageEvidence trusted = {
        .package_id = "umicom.studio",
        .signer_fingerprint = "UMICOM-RELEASE-SIGNER-01",
        .signature_present = true,
        .signature_valid = true,
        .digest_matches = true
    };
    const UmiSecurityPackageEvidence untrusted = {
        .package_id = "unknown.package",
        .signer_fingerprint = "UNKNOWN",
        .signature_present = true,
        .signature_valid = true,
        .digest_matches = true
    };
    UmiSecurityGovernanceDecision decision;
    assert(umi_studio_security_package_verify(
        centre,&trusted,UMI_SECURITY_ENV_PRODUCTION,&decision) == UMI_STATUS_OK);
    assert(decision.allowed && !decision.approval_required);
    assert(umi_studio_security_package_verify(
        centre,&untrusted,UMI_SECURITY_ENV_PRODUCTION,&decision) == UMI_STATUS_OK);
    assert(!decision.allowed && !decision.approval_required);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
